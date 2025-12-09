
#include "WiFiConn.h"
#include "SerialRecv.h"
#include "Globals.h"
#include "iFCUModbus.h"
#include <WebServer.h>

iFCUModbus ifcuModbus;

WiFiConn wifiConn;

WebServer server(80);

SerialRecv serialRecv;

void setup() {
  wifiConn.init();

  Serial.begin(115200);

  server.on("/", HTTP_POST, []() {
    Serial.print(123);
  });
  server.begin();

  ifcuModbus.init();
}

void loop() {
  server.handleClient();
}