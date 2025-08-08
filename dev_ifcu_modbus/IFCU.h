#include "Globals.h"

#ifndef IFCU_H
#define IFCU_H

class IFCU {
private:
  byte slaveId;

  static constexpr byte holdingRegisterValuesNumber = 13;
  long holdingRegisterValues[holdingRegisterValuesNumber] = {};
  long HOLDING_REGISTERS_START_ADDRESS = 40000;
  long INPUT_REGISTERS_START_ADDRESS = 30000;

  static constexpr byte inputRegisterValuesNumber = 16;
  long inputRegisterValues[inputRegisterValuesNumber] = {};


  void handleWrite4x(int addr, int value) {
    if (!mbClient.holdingRegisterWrite(this->slaveId, addr, value)) {
      Serial.println(mbClient.lastError());
    } else {
      Serial.println("Response OK");
    }
  }

public:
  IFCU(byte slaveId)
    : slaveId(slaveId){};

  void read() {

    if (!mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, HOLDING_REGISTERS_START_ADDRESS, holdingRegisterValuesNumber)) {
      Serial.println(mbClient.lastError());
    } else {
      for (uint16_t i = 0; i < holdingRegisterValuesNumber; i++) {
        uint16_t value = mbClient.read();
        Serial.print("Register ");
        Serial.print(HOLDING_REGISTERS_START_ADDRESS + i);
        Serial.print(": ");
        Serial.println(value);
      }
    }

    if (!mbClient.requestFrom(this->slaveId, INPUT_REGISTERS, INPUT_REGISTERS_START_ADDRESS, holdingRegisterValuesNumber)) {
      Serial.println(mbClient.lastError());
    } else {
      for (uint16_t i = 0; i < holdingRegisterValuesNumber; i++) {
        uint16_t value = mbClient.read();
        Serial.print("Register ");
        Serial.print(INPUT_REGISTERS_START_ADDRESS + i);
        Serial.print(": ");
        Serial.println(value);
      }
    }
  }

  void on() {
    this->handleWrite4x(40000, 1);
  }

  void off() {
    this->handleWrite4x(40000, 0);
  }
};

#endif
