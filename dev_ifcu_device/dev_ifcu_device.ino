#include "Globals.h"
#include "iFCUModbus.h"
#include "WiFiService.h"
#include "HttpService.h"
#include "Utils.h"

Utils utils;

WiFiService wifiService;

iFCUModbus ifcuModbus;

HttpService httpService;

void setup() {
  Serial.begin(115200);
  ifcuModbus.init();
}

void loop() {
  wifiService.loop();

  if (!wifiService.isConnected()) {
    return;
  }

  utils.listenSerial();

  ifcuModbus.loop();

  httpService.loop();
}