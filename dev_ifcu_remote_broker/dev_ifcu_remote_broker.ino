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
  serialBroker.setRequestValues(requestValues);
  serialBroker.setResponseValues(responseValues);

  delay(10);
}
