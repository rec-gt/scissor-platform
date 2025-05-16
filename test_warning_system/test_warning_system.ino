#include "NBIoT.h"
#include "WarningSystem.h"
#include "Utils.h"

NBIoT nbiot;

Utils utils;

WarningSystem warningSystem;

void setup() {
  Serial.begin(9600);
  nbiot.init();
}

void loop() {
  nbiot.listen();
  delay(1000);
}
