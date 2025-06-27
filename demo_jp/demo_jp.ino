#include "SystemEnums.h"
#include "ChargingSystem.h"
#include "Utils.h"

ChargingSystem chargingSystem;
Utils utils;

void setup() {
  Serial.begin(9600);
  LoRaSerial.begin(9600);
}

void loop() {
  chargingSystem.listen();
  chargingSystem.sendStatus();
}
