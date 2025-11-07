#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "PowerMeter.h"
#include "Globals.h"
#include "NBIoT.h"


PowerMeter powerMeter(100);

void setup() {
  Serial.begin(9600);

  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }

  delay(1000);
}

void loop() {
  powerMeter.read();

  delay(10);
}