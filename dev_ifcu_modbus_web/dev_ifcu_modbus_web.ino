#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "IFCU.h"
#include "Globals.h"
#include "Web.h"

Web web;

IFCU ifcu(31);

void setup() {
  Serial.begin(9600);
  WebSerial.begin(115200);

  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }

  delay(1000);
}


void loop() {
  web.listen();

  ifcu.read();
  ifcu.monitor();

  delay(10);
}