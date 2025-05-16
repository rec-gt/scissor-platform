#include "NBIoT.h"
#include "WarningSystem.h"
#include "Utils.h"

NBIoT nbiot;

Utils utils;

WarningSystem warningSystem;

void setup() {
  Serial.begin(9600);
  pinMode(10, INPUT_PULLUP);

  nbiot.init();
}

void loop() {
  nbiot.listen();
  warningSystem.listen();
  delay(1000);
}
