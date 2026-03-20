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
  jsonStr = "{";

  jsonStr += '\"';
  jsonStr += "onOff";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[0];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "mode";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[1];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "speed";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[2];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "setTemp";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[3];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "roomTemp";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[4];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "isSynced";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += isSynced;
  jsonStr += '\"';

  jsonStr += "}";

  server.send(200, "application/json", jsonStr);
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
