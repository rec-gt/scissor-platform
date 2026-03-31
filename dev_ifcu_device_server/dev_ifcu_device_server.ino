#include "Globals.h"
#include "iFCUModbus.h"
#include "iFCUServer.h"
#include "BackendServer.h"
#include "WiFiService.h"
#include "Utils.h"

Utils utils;
WiFiService wifiService;
iFCUModbus ifcuModbus;
iFCUServer ifcuServer;
BackendServer backendServer;
Memory memory;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(1024);
  ifcuModbus.init();

  utils.printMACAddress();
  wifiService.init();

  backendServer.init();


}

void loop() {
  utils.listenSerial();
  backendServer.loop();

  wifiService.loop();
  if (!wifiService.isConnected()) {
    return;
  }

  ifcuModbus.loop();
  ifcuServer.loop();

  delay(1);
}