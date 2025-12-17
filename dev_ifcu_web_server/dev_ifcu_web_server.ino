#include <HTTPClient.h>
#include "Globals.h"
#include "ESPmDNS.h"
#include "iFCUModbus.h"
#include "WiFiService.h"
#include "MDNSService.h"

WiFiService wifiService;
MDNSService mDNSService;

iFCUModbus ifcuModbus;

void setup() {
  Serial.begin(115200);
  ifcuModbus.init();
}

void loop() {
  wifiService.loop();
  if (!wifiService.isConnected()) {
    return;
  }

  mDNSService.loop();
  if (!mDNSService.isConnected()) {
    return;
  }

  ifcuModbus.handleGetAndSetHR();
  delay(500);
  ifcuModbus.handleSendIR();
  delay(500);
}
