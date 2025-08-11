#include "Globals.h"

#ifndef IFCU_H
#define IFCU_H



// #define MANUAL_COOL_SPEED_LOW 68
// #define MANUAL_COOL_SPEED_MEDIUM 69
// #define MANUAL_COOL_SPEED_HIGH 70

// #define AUTO_COOL_SPEED_LOW 68
// #define AUTO_COOL_SPEED_MEDIUM 69
// #define AUTO_COOL_SPEED_HIGH 70

// #define FAN_ONLY_SPEED_LOW 72
// #define FAN_ONLY_COOL_SPEED_MEDIUM 73
// #define FAN_ONLY_COOL_SPEED_HIGH 74

class IFCU {
private:
  byte slaveId;

  static constexpr long HOLDING_REGISTERS_START_ADDRESS = 40000;
  static constexpr long INPUT_REGISTERS_START_ADDRESS = 30000;
  static constexpr byte holdingRegisterCount = 13;
  static constexpr byte inputRegisterValuesCount = 16;
  long holdingRegisterValues[holdingRegisterCount] = {};
  long inputRegisterValues[inputRegisterValuesCount] = {};

  enum INPUT_REGISTER {
    FW_VER,
    SYSTEM_MODE,
    SYSTEM_OPTION,
    OPERATION_MODE,
    MANUAL_MODE_FAN_SPEED,
    ROOM_TEMP,
    SET_TEMP,
  };

  void handleWrite4x(unsigned int addr, unsigned int value) {
    if (!mbClient.holdingRegisterWrite(this->slaveId, addr, value)) {
      Serial.println(mbClient.lastError());
    } else {
      Serial.println("Response OK");
    }
  }

  bool checkOnOff() {
    // 0 = off, not 0 = on, map to "0/false" & "1/true"
    return (this->inputRegisterValues[SYSTEM_MODE] & 0b01000000) != 0;
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

    // for (uint16_t i = 0; i < holdingRegisterCount; i++) {
    //   Serial.print(this->holdingRegisterValues[i]);
    //   Serial.print(", ");
    // }
    // Serial.println();

    // for (uint16_t i = 0; i < inputRegisterValuesCount; i++) {
    //   Serial.print(this->inputRegisterValues[i]);
    //   Serial.print(", ");
    // }
    // Serial.println();

    // Serial.println(this->inputRegisterValues[OPERATION_MODE]);
    // Serial.println(this->inputRegisterValues[ROOM_TEMP]);
    // Serial.println(this->inputRegisterValues[SET_TEMP]);
    // Serial.println(this->inputRegisterValues[MANUAL_MODE_FAN_SPEED]);
    Serial.println(this->checkOnOff());
  }

  // ===== handle operations =====
  void handleOnOff(IFCU_ENUMS toggle) {
    this->handleWrite4x(40000, toggle == IFCU_ON ? 1 : 0);
  }

  void handleChangeMode(IFCU_ENUMS mode) {
    switch (mode) {
      case IFCU_MODE_AUTO_COOL:
        this->handleWrite4x(40002, 0);
        break;
      case IFCU_MODE_MANUAL_COOL:
        this->handleWrite4x(40002, 1);
        break;
      case IFCU_MODE_FAN_ONLY:
        this->handleWrite4x(40002, 2);
        break;
      case IFCU_MODE_AUTO_HEAT:
        this->handleWrite4x(40002, 4);
        break;
      case IFCU_MODE_MANUAL_HEAT:
        this->handleWrite4x(40002, 5);
        break;
      default:
        this->handleWrite4x(40002, 0);
    }
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

  void handleChangeSetPointTemp(IFCU_ENUMS action, byte step = 50) {
    unsigned int currSetPointTemp = this->inputRegisterValues[SET_TEMP];

    switch (action) {
      case IFCU_ACTION_INCREASE_TEMP:
        this->handleWrite4x(40004, currSetPointTemp + step);
        break;
      case IFCU_ACTION_DECREASE_TEMP:
        this->handleWrite4x(40004, currSetPointTemp - step);
        break;
    }
  }
};

#endif
