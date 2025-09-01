#include "SerialBroker.h"
#include "Globals.h"

SerialBroker serialBroker;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  serialBroker.listenByte();
  serialBroker.handleRecvBuffer();

  Serial.print("\r\n===================\r\n");
  for (int i = 0; i < 4; i++) {
    Serial.print(requestValues[i]);
    Serial.print(", ");
  }
  delay(500);
}
