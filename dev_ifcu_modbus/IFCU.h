#include "Globals.h"

#ifndef IFCU_H
#define IFCU_H

class IFCU {
private:
  byte slaveId;

  static constexpr long HOLDING_REGISTERS_START_ADDRESS = 40000;
  static constexpr long INPUT_REGISTERS_START_ADDRESS = 30000;
  static constexpr byte holdingRegisterCount = 13;
  static constexpr byte inputRegisterValuesCount = 16;
  long holdingRegisterValues[holdingRegisterCount] = {};
  long inputRegisterValues[inputRegisterValuesCount] = {};


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
    if (!mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, HOLDING_REGISTERS_START_ADDRESS, holdingRegisterCount)) {
      Serial.println(mbClient.lastError());
    } else {
      for (uint16_t i = 0; i < holdingRegisterCount; i++) {
        this->holdingRegisterValues[i] = mbClient.read();
      }
    }

    if (!mbClient.requestFrom(this->slaveId, INPUT_REGISTERS, INPUT_REGISTERS_START_ADDRESS, inputRegisterValuesCount)) {
      Serial.println(mbClient.lastError());
    } else {
      for (uint16_t i = 0; i < inputRegisterValuesCount; i++) {
        this->inputRegisterValues[i] = mbClient.read();
      }
    }

    for (uint16_t i = 0; i < holdingRegisterCount; i++) {
      Serial.print(this->holdingRegisterValues[i]);
      Serial.print(", ");
    }
    Serial.println();

    for (uint16_t i = 0; i < inputRegisterValuesCount; i++) {
      Serial.print(this->inputRegisterValues[i]);
      Serial.print(", ");
    }
    Serial.println();
  }

  // ===== handle operations =====
  void handleOnOff(IFCU_ENUMS toggle) {
    this->handleWrite4x(40000, toggle == IFCU_ON ? 1 : 0);
  }

  void handleChangeMode(IFCU_ENUMS mode) {
    this->handleWrite4x(40002, mode);
  }

  void handleChangeFanSpeed(IFCU_ENUMS fanSpeed) {
    switch (fanSpeed) {
      case IFCU_FAN_SPEED_LOW:
        this->handleWrite4x(40003, 0);
        break;
      case IFCU_FAN_SPEED_MEDIUM:
        this->handleWrite4x(40003, 1);
        break;
      case IFCU_FAN_SPEED_HIGH:
        this->handleWrite4x(40003, 2);
        break;
      case IFCU_FAN_SPEED_TEN_SPEED:
        this->handleWrite4x(40003, 3);
        break;
      default:
        this->handleWrite4x(40003, 1);
    }
  }
};

#endif
