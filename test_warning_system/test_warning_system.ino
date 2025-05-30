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
  nbiot.init();
}

void loop() {
  nbiot.listen();
  warningSystem.listen();
  warningSystem.print();
 
  delay(1000);
}
