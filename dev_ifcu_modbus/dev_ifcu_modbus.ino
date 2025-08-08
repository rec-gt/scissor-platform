#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "IFCU.h"

IFCU ifcu1(31);

void setup() {
  Serial.begin(9600);

  // Start Serial1 as the Modbus RTU communication port
  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }

  handleWrite4x(31, 40000, 0);
  handleWrite4x(31, 40003, 0);
  handleWrite4x(31, 40005, 2225);
}

void loop() {
}