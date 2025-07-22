#include "NBIoT.h"
#include "Enums.h"
#include "Watchdog.h"
#include "AsyncTimer.h"

NBIoT nbiot;
Watchdog watchdog;
AsyncTimer timer(15 * 1000);

void setup() {
  Serial.begin(9600);
  NBIoT_Serial.begin(9600);
  pinMode(9, OUTPUT);
  delay(300);
  digitalWrite(9, HIGH);
}

void loop() {
  nbiot.start();
  nbiot.waitMsg();

  if (timer.isExpired()) {
    nbiot.publish();
    timer.refresh();
  }

  delay(10);
}
