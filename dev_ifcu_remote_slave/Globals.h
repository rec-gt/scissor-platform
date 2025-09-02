#ifndef GLOBALS_H
#define GLOBALS_H

#define DEVICE_ID 1
#define SLAVE_ID 31

#define REQUEST_VALUES_LEN 4
#define RESPONSE_VALUES_LEN 7

uint16_t requestValues[REQUEST_VALUES_LEN] = {
  0,  // on/off
  0,  // set point temp
  0,  // mode
  0,  // speed
};

uint16_t responseValues[RESPONSE_VALUES_LEN] = {
  0,  // on/off
  0,  // mode
  0,  // speed
  0,  // room temp
  0,  // set point temp
  0,  // lower temp limit
  0,  // upper temp limit
};

void printRequestValues() {
  Serial.print("\r\n===================\r\n");
  for (int i = 0; i < 4; i++) {
    Serial.print(requestValues[i]);
    Serial.print(", ");
  }
  Serial.println();
}

void printResponseValues() {
  Serial.print("\r\n===================\r\n");
  for (uint16_t i = 0; i < 7; i++) {
    Serial.print(responseValues[i]);
    Serial.print(", ");
  }
  Serial.println();
}

#endif