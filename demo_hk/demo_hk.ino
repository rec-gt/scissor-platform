#include "SystemEnums.h"
#include "ChargingSystem.h"
#include "Utils.h"
#include <avr/wdt.h>

ChargingSystem chargingSystem;
Utils utils;

void setup() {
  Serial.begin(9600);
  LoRaSerial.begin(9600);
  wdt_enable(WDTO_8S);
}

void loop() {
  chargingSystem.listen();
  wdt_reset();
  delay(10);
  delay(1000);
}
