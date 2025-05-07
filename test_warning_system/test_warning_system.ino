#include "NBIoT.h"
#include "Utils.h"

NBIoT nbiot;

Utils utils;

void setup() {
  Serial.begin(9600);
  nbiot.init();
}

void loop() {
  nbiot.listen();
  delay(1000);
}
