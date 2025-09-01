#include "SerialBroker.h"
#include "ModbusBroker.h"
#include "Globals.h"

SerialBroker serialBroker;
ModbusBroker modbusBroker;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  serialBroker.listenByte();
  serialBroker.handleRecvBuffer();
  modbusBroker.loop();

  delay(500);
}
