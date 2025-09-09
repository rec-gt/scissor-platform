#include "NBIoT.h"
#include "Enums.h"
#include "Watchdog.h"
#include "AsyncTimer.h"

NBIoT nbiot;

void setup() {
  Serial.begin(9600);
  NBIoTSerial.begin(9600);

  // === For NBIoT ===
  nbiot.debug();
  nbiot.init(true);
}

void stressTest() {
  // make sure stress test will not exceed 1000ms execution time
  for (size_t i = 0; i < 20; i++) {
    Serial.print(" [STRESS TEST] ");
    Serial.flush();
    if (i % 10 == 0) {
      Serial.println();
      Serial.flush();
    }
  }
}

void loop() {
  nbiot.loop();
  stressTest();
  delay(10);
}
