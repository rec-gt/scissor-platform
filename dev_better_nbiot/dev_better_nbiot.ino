#include "NBIoT.h"
#include "Enums.h"
#include "Watchdog.h"
#include "AsyncTimer.h"

NBIoT nbiot;
Watchdog watchdog;
AsyncTimer timer1(30UL * 1000UL);

void setup() {
  Serial.begin(9600);
  NBIOT_SERIAL.begin(9600);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  delay(300);
  nbiot.init();
}

void loop() {
  while (1) {
    nbiot.listen();
    if (nbiot.finishInit) {
      break;
    } else {
      delay(10);
    }
  }


  if (timer1.isExpired()) {
    nbiot.publish();
    timer1.refresh();
  }

  delay(10);
}
