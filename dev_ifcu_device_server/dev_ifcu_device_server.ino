#include "Globals.h"
#include "iFCUModbus.h"
#include "iFCUServer.h"
#include "WiFiService.h"
#include "Utils.h"

Utils utils;
WiFiService wifiService;
iFCUModbus ifcuModbus;
iFCUServer ifcuServer;

void setup() {
  Serial.begin(115200);
  ifcuModbus.init();
}

void loop() {
  utils.listenSerial();

  wifiService.loop();
  if (!wifiService.isConnected()) {
    return;
  }

  ifcuModbus.loop();
  ifcuServer.loop();
}