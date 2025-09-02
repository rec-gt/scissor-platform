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
    server.on("/get", [this]() {
      server.send(200, "text/plain", this->localAndRemoteData);
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
      localAndRemoteData = "";
      localAndRemoteData += String(requestValues[0]);
      localAndRemoteData += ",";
      localAndRemoteData += String(requestValues[1]);
      localAndRemoteData += ",";
      localAndRemoteData += String(requestValues[2]);
      localAndRemoteData += ",";
      localAndRemoteData += String(requestValues[3]);
      localAndRemoteData += "-";
      localAndRemoteData += String(responseValues[0]);
      localAndRemoteData += ",";
      localAndRemoteData += String(responseValues[1]);
      localAndRemoteData += ",";
      localAndRemoteData += String(responseValues[2]);
      localAndRemoteData += ",";
      localAndRemoteData += String(responseValues[3]);
      localAndRemoteData += ",";
      localAndRemoteData += String(responseValues[4]);
      localAndRemoteData += ",";
      localAndRemoteData += String(responseValues[5]);
      localAndRemoteData += ",";
      localAndRemoteData += String(responseValues[6]);
      Serial.println(localAndRemoteData);
    }

    server.handleClient();
  }

  ~BackendServer(){};
};

#endif