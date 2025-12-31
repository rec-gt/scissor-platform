#include "IoT.h"

IoT iot;

void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);

  /*=== IoT ===*/
  iot.init(true);
}

void loop() {
  /*=== Register IoT ===*/
  iot.loop();
}