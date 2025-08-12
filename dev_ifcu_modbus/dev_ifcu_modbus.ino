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

  delay(1000);
}

void loop() {
  nbiot.loop();

  ifcu.read();
  ifcu.monitor();
  
  delay(10);
}