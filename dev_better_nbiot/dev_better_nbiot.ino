#include "NBIoT.h"
#include "Enums.h"
#include "Watchdog.h"
#include "AsyncTimer.h"

NBIoT nbiot;
Watchdog watchdog(30000);

void setup() {
  Serial.begin(9600);
  NBIoTSerial.begin(9600);

  // === NBIOT ===
  nbiot.debugMode();

  nbiot.init();
}

void loop() {
  nbiot.loop();
  
  for (size_t i = 0; i < 50; i++) {
    Serial.print("STRESS TEST ");
    delay(10);
  }
  
  delay(10);
}
