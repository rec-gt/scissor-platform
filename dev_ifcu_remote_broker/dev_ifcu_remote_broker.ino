#include "SerialBroker.h";

#define REQUEST_VALUES_LEN 4
#define RESPONSE_VALUES_LEN 7

SerialBroker serialBroker;

int requestValues[REQUEST_VALUES_LEN] = {
  0,  // on/off
  0,  // set point temp
  0,  // mode
  0,  // speed
};

int responseValues[RESPONSE_VALUES_LEN] = {
  0,  // on/off
  0,  // room temp
  0,  // set point temp
  0,  // mode
  0,  // speed
  0,  // lower temp limit
  0,  // upper temp limit
};

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  serialBroker.listen();
  serialBroker.setRequestValues(requestValues);
  serialBroker.setResponseValues(responseValues);

  delay(10);
}
