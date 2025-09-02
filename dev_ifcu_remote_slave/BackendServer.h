#include <WebServer.h>

#ifndef BackendServer_h
#define BackendServer_h

WebServer server(80);

class BackendServer {
public:
  BackendServer(){};

  void init() {
    server.on("/get", HTTP_GET, []() {
      server.send(200, "text/plain", "123");
    });

    server.on("/set", HTTP_GET, []() {
      if (server.hasArg("value")) {
        String valStr = server.arg("value");
        currentValue = valStr.toInt();
        server.send(200, "text/plain", "Value set to: " + String(currentValue));
        Serial.println("GET /set - Set to: " + String(currentValue));
      }
    });
  };

  void loop() {
    server.handleClient();
  }

  ~BackendServer(){};
};

#endif