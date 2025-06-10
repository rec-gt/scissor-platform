#include "NBIoT.h"
#include "Speaker.h"
#include "AlarmSystem.h"
#include "Enums.h"
#include "Watchdog.h"

NBIoT nbiot;
Speaker speaker;
AlarmSystem alarmSystem;
Watchdog watchdog;

void setup() {
  Serial.begin(9600);
  NBIoT_Serial.begin(9600);
  delay(300);
}

void loop() {
  watchdog.listen();
  nbiot.listen();
  nbiot.waitMsg();
  alarmSystem.listen();
}
