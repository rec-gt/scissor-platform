
#include "Globals.h"
#include "SerialBroker.h"
#include "ModbusBroker.h"

SerialBroker serialBroker;
ModbusBroker modbusBroker;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);  // send response to slave
  Serial2.begin(9600);  // receive request from slave

  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }
}

void loop() {
  serialBroker.loop();
  modbusBroker.loop();
  delay(10);
}
