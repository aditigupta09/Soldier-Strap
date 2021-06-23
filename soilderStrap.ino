#include "HTTPSRedirectY.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include "AESLib.h"

SoftwareSerial GPS(D5, D6);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
TinyGPS gps;
void gpsdump(TinyGPS &gps);
unsigned long fix_age;
bool feedgps();
void getGPS();
long lat, lon;
float LAT, LON;

const char* ssid = "IOTADMIN"; //SSID for connection for WIFI
const char* password = "admin@12345"; //Password for the WIFI
const char* GScriptId = "AKfycbwJN6KCn-Ix7OBBn82o43O1coG0JlBmHDFik3D1yAJNjrU_BFPNMrtYoQ"; //Script ID for posting data for google server
const char* host = "script.google.com";
String url = String("/macros/s/") + GScriptId + "/exec?";
const int httpsPort = 443;
const char* googleRedirHost = "script.googleusercontent.com";
HTTPSRedirectY client(httpsPort);
AESLib aesLib;

byte key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }; //Unique keys changes for every upload
byte my_iv[N_BLOCK] = {0,0,1,0,1,0,0,3,0,5,0,2,0,0,7,0}; //Block values NOTE: any changes can create failure to create proper encryption
   
void setup() {
  Serial.begin(9600);
  GPS.begin(9600);
  mlx.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  bool flag = false;
  client.setInsecure();
  for (int i = 0; i < 5; i++) {
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
  }
  pinMode(A0, INPUT);
  aesLib.gen_iv(my_iv); //Preparing AES library for further use
}

void loop() {
  gps.get_position(&lat, &lon, &fix_age);
  getGPS();
  int sensorValue = analogRead(A0);
  sensorValue = (int)sensorValue/720;

  //printing the readings in the Serial Monitor for DEBUG
  //  Serial.print("sensor = ");
  //  Serial.print(sensorValue);
  //  Serial.println(mlx.readAmbientTempF());
  //  Serial.println(LAT / 1000000, 7);
  //  Serial.println(LON / 1000000, 7);
  postData("ss", (String)mlx.readAmbientTempF(), (String)(LAT / 1000000), (String)(LON / 1000000), (String)sensorValue);
  delay(10000);
}

//Posting data over the server
void postData(String tag, String value, String value1, String value2, String value3) {
  if (!client.connected()) {
    client.connect(host, httpsPort); //connecting to client
  }
  //creating final encrypted value url to be posted
  String urlFinal = url + "tag=" + tag + "&f1=" + aesLib.encrypt(value1, key, my_iv) + "&f2=" + aesLib.encrypt(value2, key, my_iv) + "&f3=" + aesLib.encrypt(value, key, my_iv) + "&f4=" + aesLib.encrypt(value3, key, my_iv) + "&f5=" + aesLib.encrypt(value, key, my_iv) + "&f6=" + aesLib.encrypt(value, key, my_iv) + "&f7=" + aesLib.encrypt(value, key, my_iv);
  client.printRedir(urlFinal, host, googleRedirHost); //Calling URL
  //  Serial.println("Done");
}

//Getting the GPS values
void getGPS() {
  bool newdata = false;
  unsigned long start = millis();
  // Every 1 seconds we print an update
  while (millis() - start < 1000) {
    if (feedgps ()) {
      newdata = true;
    }
  }
  if (newdata){
    gpsdump(gps);
  }

}

//For GPS feed values
bool feedgps() {
  while (GPS.available()) {
    if (gps.encode(GPS.read()))
      return true;
  }
  return 0;
}

void gpsdump(TinyGPS &gps) {
  //byte month, day, hour, minute, second, hundredths;
  gps.get_position(&lat, &lon);
  LAT = lat;
  LON = lon;
  {
    feedgps(); // If we don't feed the gps during this long routine,
    //we may drop characters and get checksum errors
  }
}
