#include "HTTPSRedirectY.h"

//#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

HTTPSRedirectY::HTTPSRedirectY(const int p, const char* fp, bool c) 
    : httpsPort(p), redirFingerprint(fp), fpCheck(c){
}

HTTPSRedirectY::HTTPSRedirectY(const int p) 
    : httpsPort(p){
      fpCheck = false;
}

HTTPSRedirectY::~HTTPSRedirectY(){ 
}

bool HTTPSRedirectY::printRedir(String& url, const char* host, const char* redirHost){
  return printRedir(url.c_str(), host, redirHost);
}

bool HTTPSRedirectY::printRedir(const char* url, const char* host, const char* redirHost){

  int redirFlag = false;

  // Check if connection to host is alive
  if (!connected()){
    Serial.println("Error! Not connected to host.");
    return false;
  }
  String Request = createRequest(url, host);

  DPRINTLN(Request);
  // make request
  print(Request);

  String line;
  String redirUrl;
  
  DPRINTLN("Detecting re-direction.");
  DPRINTLN(redirHost);
  
  while (connected()) {
    line = readStringUntil('\n');
    DPRINTLN(line);
    if (line == "\r") {
      DPRINTLN("END OF HEADER");
      //DPRINTLN(line);
      break;
    }
    
    if (find("Location: ") ){
      find((char *)redirHost);
      DPRINTLN("Found re-direction URL!");
      redirUrl = readStringUntil('\n');
      redirFlag = true;
      break;
    }
  }
  
  DPRINTLN("Body:\n");
  if (verboseInfo)
    fetchData(true, false);
  else
    flush();
    
  if (!redirFlag){
    DPRINTLN("No re-direction URL found in header.");
    return false;
  }
  //flush();
  //stop();

  DPRINTLN("Redirected URL:");
  DPRINTLN(redirUrl);
  
  Request = createRequest(redirUrl.c_str(), redirHost);
                          
  DPRINTLN("Connecting to:");
  DPRINTLN(redirHost);

  if (!connect(redirHost, httpsPort)) {
    Serial.println("Connection to re-directed host failed!");
    return 0;
  }

  if (fpCheck){
    if (verify(redirFingerprint, redirHost)) {
      Serial.println("Re-directed host certificate match.");
    } else {
      Serial.println("Re-directed host certificate mis-match");
    }
  }
  
  DPRINTLN("Requesting re-directed URL.");
  DPRINTLN(Request);

  // Make request
  print(Request);
  
  DPRINTLN("Final Response:");
  
  fetchData(false, true);

  fetchData(true, false);

  flush();
  //stop();

  return true;
}

String HTTPSRedirectY::createRequest(const char* url, const char* host){
  return String("GET ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "User-Agent: ESP8266\r\n" +
                          (keepAlive ? "" : "Connection: close") + 
                          "\r\n\r\n";
  
}

void HTTPSRedirectY::fetchData(bool disp, bool header){
  String line;
  Data="";
  while (connected()) {
    line = readStringUntil('\n');
    
    if (disp){
      Data+=line;
      Serial.println(line);
    }
      
    if (line == "\r") {
      if (disp){
        if (header)
          DPRINTLN("END OF HEADER");
        else
          DPRINTLN("END OF RESPONSE");
          //DPRINTLN(line);
      }
      break;
    }
  }
}
