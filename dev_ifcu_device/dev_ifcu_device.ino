#include <HTTPClient.h>
#include "Globals.h"
#include "ESPmDNS.h"
#include "iFCUModbus.h"
// #include "WiFiService.h"
#include "MDNSService.h"

// WiFiService wifiService;
MDNSService mDNSService;

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

  // mDNSService.loop();
  // if (!mDNSService.isConnected()) {
  //   return;
  // }

  // ifcuModbus.handleGetAndSetHR();
  // delay(500);

  while (Serial.available()) {
    char c = Serial.read();
    QUEUE += c;
  }


  ifcuModbus.readDataFromDevice();
  delay(1000);
  Serial.println(QUEUE);
  ifcuModbus.syncWithQueue();
  Serial.println(QUEUE);
  delay(1000);
  ifcuModbus.writeDataToDevice();
  delay(1000);
}
