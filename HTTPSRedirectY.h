#include <WiFiClientSecure.h>

class HTTPSRedirectY : public WiFiClientSecure {
  private:
    const int httpsPort;
    const char* redirFingerprint;
    bool fpCheck = false;
    bool keepAlive = true;
    bool verboseInfo = false;
    
  public:
    String Data="";
    HTTPSRedirectY(const int, const char*, bool);
    HTTPSRedirectY(const int);
    ~HTTPSRedirectY();

    bool printRedir(const char*, const char*, const char*);
    bool printRedir(String&, const char*, const char*);
    String createRequest(const char*, const char*);
    void fetchData(bool, bool);
  
};
