#include "Globals.h"
#include "SerialBroker.h"
#include "WiFiBroker.h"

SerialBroker serialBroker;
WiFiBroker wifiBroker;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, 18, 19);  // receive status from modbus-broker
  Serial2.begin(9600, SERIAL_8N1, 16, 17);  // send request to modbus-broker

  wifiBroker.init();
}

void loop() {
  serialBroker.loop();
  wifiBroker.loop();
  delay(10);
}
