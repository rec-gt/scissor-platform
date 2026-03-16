#include <HTTPClient.h>
#include "Globals.h"
#include "iFCUModbus.h"
// #include "WiFiService.h"

// WiFiService wifiService;

iFCUModbus ifcuModbus;

void setup() {
  Serial.begin(115200);
  ifcuModbus.init();
}

void loop() {
  // wifiService.loop();
  // if (!wifiService.isConnected()) {
  //   return;
  // }

  while (Serial.available()) {
    char c = Serial.read();
    QUEUE += c;
  }

  // http.loop(); 

  ifcuModbus.loop();
}
