#ifndef IFCU_SERVER_H
#define IFCU_SERVER_H

#include <WebServer.h>

WebServer server(80);

void handleSet() {
  if (server.hasArg("cmd")) {
    String cmd = server.arg("cmd");
    QUEUE += cmd;
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing parameters.");
  }
}

void handleGet() {
  server.send(200, "application/json", "{\"mode\":\"0\",\"isSynced\":\"0\"}");
}

class iFCUServer {

private:
  void restartWebServer() {
    Serial.println("Restarting web server...");
    server.stop();
    Serial.println("Web server stopped.");
    delay(1000);
    server.enableCORS();
    server.on("/set", HTTP_GET, handleSet);
    server.on("/get", HTTP_GET, handleGet);
    server.begin();
    Serial.println("Web server started!");
  }

public:
  void loop() {
    if (restartServerFlag) {
      restartWebServer();
      restartServerFlag = false;
    }

    server.handleClient();
  }
};

extern iFCUServer ifcuServer;

#endif
