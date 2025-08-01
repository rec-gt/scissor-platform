#include "NBIoT.h"
#include "Enums.h"
#include "Watchdog.h"
#include "AsyncTimer.h"

NBIoT nbiot;
Watchdog watchdog(30000);
AsyncTimer timer1(30000);

void setup() {
  Serial.begin(9600);
  NBIoTSerial.begin(9600);

  // === NBIOT ===
  nbiot.debugMode();

  nbiot.init();
}

void loop() {
  nbiot.loop();
  delay(10);
}
