#include "Globals.h"
#include "iFCUModbus.h"
#include "iFCUServer.h"
#include "WiFiService.h"
#include "Utils.h"

Utils utils;
WiFiService wifiService;
iFCUModbus ifcuModbus;
iFCUServer ifcuServer;
Memory memory;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(1024);
  ifcuModbus.init();

  utils.printMACAddress();
  wifiService.setAP();
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