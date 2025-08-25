#include "SystemEnums.h"
#include "ChargingSystem.h"
#include "Utils.h"
#include "NBIoT.h"
#include <avr/wdt.h>

ChargingSystem chargingSystem;
NBIoT nbiot;
Utils utils;

void setup() {
  Serial.begin(9600);
  NBIoTSerial.begin(9600);
  wdt_enable(WDTO_8S);
}

void loop() {
  chargingSystem.listen();
  wdt_reset();
  delay(1000);
}
