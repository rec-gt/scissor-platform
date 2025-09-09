#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "IFCU.h"
#include "Globals.h"
#include "BLE.h"
#include "NBIoT.h"

BLE ble;
NBIoT nbiot;

IFCU ifcu(31);

void setup() {
  Serial.begin(9600);

  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }

  BLESerial.begin(9600);
  delay(1000);
}


void loop() {
  ble.listen();

  ifcu.read();
  ifcu.monitor();

  delay(10);
}