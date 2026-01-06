#include "IoT.h"
#include "AsyncTimer.h"

IoT iot;
AsyncTimer timer(1000);

void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);
  SerialIoT.begin(115200);

  /*=== IoT ===*/
  iot.init();
  iot.debug();
}

void loop() {
  /*=== Register IoT ===*/
  iot.loop();
  if (timer.autoExpired(300)) {
    Serial.println(iot.connState);
  }
}