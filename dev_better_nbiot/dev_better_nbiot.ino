#include "NBIoT.h"
#include "Enums.h"
#include "Watchdog.h"
#include "AsyncTimer.h"

NBIoT nbiot;
Watchdog watchdog;
AsyncTimer timer1(30000);
AsyncTimer timer2(5000);

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

  if (timer1.isExpired()) {
    nbiot.publish();
    timer1.refresh();
  }

  if (timer2.isExpired()) {
    nbiot.interact();
    timer2.refresh();
  }

  delay(10);
}
