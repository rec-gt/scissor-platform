#include "SystemEnums.h"
#include "NBIoT.h"
#include "WarningSystem.h"
#include "Utils.h"

NBIoT nbiot;

Utils utils;

WarningSystem warningSystem;

SystemStatus SYSTEM_STATUS;

void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT);

  nbiot.init();
}

void loop() {
  nbiot.listen();
  warningSystem.listen();
  warningSystem.execAlarmTasks();
  delay(1000);
}
