#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#ifndef Globals_H
#define Globals_H

#define REQUEST_VALUES_LEN 4
#define RESPONSE_VALUES_LEN 7

#define SLAVE_ID 31

RS485Class rs485(Serial3, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

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
