#include <WebServer.h>
#include "Globals.h"

#ifndef BackendServer_h
#define BackendServer_h

AutoTimer serverTimer;

WebServer server(80);

class BackendServer {
private:
  String localRemoteData = "";

public:
  BackendServer(){};

  void init() {
    server.on("/get", [this]() {
      server.send(200, "text/plain", this->localRemoteData);
    });

    server.on("/set", []() {
      if (server.hasArg("onoff")) {
        String valStr = server.arg("onoff");
        requestValues[0] = valStr.toInt();
        Serial.println(valStr);
      }
      if (server.hasArg("mode")) {
        String valStr = server.arg("mode");
        requestValues[2] = valStr.toInt();
        Serial.println(valStr);
        Serial.println(requestValues[2]);
      }
      if (server.hasArg("speed")) {
        String valStr = server.arg("speed");
        requestValues[3] = valStr.toInt();
        Serial.println(valStr);
      }
    });
    server.begin();
  };

  void loop() {
    if (serverTimer.autoExpire(1000)) {
      this->localRemoteData = "[";
      this->localRemoteData += String(requestValues[0]);
      this->localRemoteData += ",";
      this->localRemoteData += String(requestValues[1]);
      this->localRemoteData += ",";
      this->localRemoteData += String(requestValues[2]);
      this->localRemoteData += ",";
      this->localRemoteData += String(requestValues[3]);
      this->localRemoteData += ",";
      this->localRemoteData += String(responseValues[0]);
      this->localRemoteData += ",";
      this->localRemoteData += String(responseValues[1]);
      this->localRemoteData += ",";
      this->localRemoteData += String(responseValues[2]);
      this->localRemoteData += ",";
      this->localRemoteData += String(responseValues[3]);
      this->localRemoteData += ",";
      this->localRemoteData += String(responseValues[4]);
      this->localRemoteData += ",";
      this->localRemoteData += String(responseValues[5]);
      this->localRemoteData += ",";
      this->localRemoteData += String(responseValues[6]);
      this->localRemoteData += String(responseValues[6]);
      Serial.println(localRemoteData);
    }

    server.handleClient();
  }

  ~BackendServer(){};
};

#endif