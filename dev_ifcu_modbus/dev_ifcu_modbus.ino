#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "IFCU.h"
#include "Globals.h"
#include "NBIoT.h"

NBIoT nbiot;

IFCU ifcu(31);

void setup() {
  Serial.begin(9600);

  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }

  nbiot.debug();
  nbiot.init(1);
}

void loop() {
  ifcu.handleOnOff(IFCU_ON);
  delay(1000);
  ifcu.handleChangeMode(IFCU_MODE_FAN_ONLY);
  delay(1000);
  ifcu.handleChangeFanSpeed(IFCU_FAN_SPEED_MEDIUM);
  delay(1000);

  ifcu.read();
  delay(1000);
}