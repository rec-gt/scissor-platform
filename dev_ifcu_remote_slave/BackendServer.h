#include <WebServer.h>

#ifndef BackendServer_h
#define BackendServer_h

AutoTimer serverTimer;

WebServer server(80);

class BackendServer {
private:
  String localAndRemoteData = "";

public:
  BackendServer(){};

  void init() {
    server.on("/get", []() {
      server.send(200, "text/plain", "123123123123");
    });

    server.on("/set", []() {
      if (server.hasArg("value")) {
        // String valStr = server.arg("value");
        // currentValue = valStr.toInt();
        // server.send(200, "text/plain", "Value set to: " + String(currentValue));
        // Serial.println("GET /set - Set to: " + String(currentValue));
      }
    });
    server.begin();
  };

  void loop() {
    if (serverTimer.autoExpire(1000)) {
      this->localAndRemoteData = "";
      this->localAndRemoteData += String(responseValues[0]);
      this->localAndRemoteData += ",";
      this->localAndRemoteData += String(responseValues[1]);
      this->localAndRemoteData += ",";
      this->localAndRemoteData += String(responseValues[2]);
      this->localAndRemoteData += ",";
      this->localAndRemoteData += String(responseValues[3]);
      this->localAndRemoteData += ",";
      this->localAndRemoteData += String(responseValues[4]);
      this->localAndRemoteData += ",";
      this->localAndRemoteData += String(responseValues[5]);
      this->localAndRemoteData += ",";
      this->localAndRemoteData += String(responseValues[6]);
    }

    server.handleClient();
  }

  ~BackendServer(){};
};

#endif