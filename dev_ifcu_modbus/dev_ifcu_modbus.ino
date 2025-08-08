#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "IFCU.h"
#include "Globals.h"

IFCU ifcu1(31);

void setup() {
  Serial.begin(9600);

  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }
}

void loop() {
}