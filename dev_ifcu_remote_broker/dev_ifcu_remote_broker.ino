#include "SerialBroker.h";

SerialBroker serialBroker;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  serialBroker.listen();
}
