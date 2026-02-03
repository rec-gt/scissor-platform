#include "IoT.h"
#include "AsyncTimer.h"
#include <EEPROM.h>

IoT iot;
AsyncTimer timer(1000);

void setup() {
  Serial.begin(9600);

  int prevSensorThresholdDistance = EEPROM.read(0);
  Serial.print(prevSensorThresholdDistance);
}

void loop() {
}