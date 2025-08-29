#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#ifndef ModbusBroker_H
#define ModbusBroker_H

#define SLAVE_ID 31

RS485Class rs485(Serial3, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

class ModbusBroker {
private:
  void handleWrite4x(unsigned int addr, unsigned int value) {
    if (!mbClient.holdingRegisterWrite(SLAVE_ID, addr, value)) {
      Serial.println(mbClient.lastError());
    } else {
      Serial.println("Response OK");
    }
  }

public:
  ModbusBroker(){};

  void listen() {
    this->read();
    if (millis() - this->prevMillis2 > 500) {
      this->monitor(webClient.readCmd());
      this->prevMillis2 = millis();
    }
  }

  // ===== read data from control box =====
  void read() {
    if (millis() - this->prevMillis > 1000) {
      if (!mbClient.requestFrom(this->slaveId, INPUT_REGISTERS, INPUT_REGISTERS_START_ADDRESS, INPUT_REGISTER_VALUES_COUNT)) {
        Serial.println(mbClient.lastError());
      } else {
        for (uint16_t i = 0; i < INPUT_REGISTER_VALUES_COUNT; i++) {
          this->inputRegisterValues[i] = mbClient.read();
        }
      }

      this->prepareSendBuffer();
      webClient.sendBuffer();

      // === debug ===
      // for (uint16_t i = 0; i < INPUT_REGISTER_VALUES_COUNT; i++) {
      //   Serial.print(this->inputRegisterValues[i]);
      //   Serial.print(", ");
      // }
      // Serial.println();

      this->prevMillis = millis();
    }
  }

  // ===== monitor upcoming command =====
  void monitor(String msg) {

    // if (msg.length() <= 0) {
    //   return;
    // }
    // int cmd = msg.toInt();

    int cmd = -1;

    if (que.length <= 0) {
      return;
    }

    cmd = que.pop();

    Serial.print(" CMD: ");
    Serial.print(cmd);

    switch (cmd) {
      case IFCU_ON:
        this->handleWrite4x(40000, 1);
        break;
      case IFCU_OFF:
        this->handleWrite4x(40000, 0);
        break;
      case IFCU_MODE_AUTO_COOL:
        this->handleWrite4x(40002, 0);
        break;
      case IFCU_MODE_MANUAL_COOL:
        this->handleWrite4x(40002, 1);
        break;
      case IFCU_MODE_FAN_ONLY:
        this->handleWrite4x(40002, 2);
        break;
      case IFCU_FAN_SPEED_LOW:
        this->handleWrite4x(40003, 0);
        break;
      case IFCU_FAN_SPEED_MEDIUM:
        this->handleWrite4x(40003, 1);
        break;
      case IFCU_FAN_SPEED_HIGH:
        this->handleWrite4x(40003, 2);
        break;
      case IFCU_ACTION_INCREASE_TEMP:
        this->defaultSetTemp += 50;
        this->handleWrite4x(40004, this->defaultSetTemp);
        // this->handleWrite4x(40004, this->inputRegisterValues[INPUT_REGISTER_SET_TEMP] + 50);
        break;
      case IFCU_ACTION_DECREASE_TEMP:
        this->defaultSetTemp -= 50;
        this->handleWrite4x(40004, this->defaultSetTemp);
        // this->handleWrite4x(40004, this->inputRegisterValues[INPUT_REGISTER_SET_TEMP] - 50);
        break;
    }
  }

  // ===== handle operations =====
  void handleOnOff(IFCU_ENUMS toggle) {
    this->handleWrite4x(40000, toggle == IFCU_ON ? 1 : 0);
  }

  void prepareSendBuffer() {
    webClientSendBytes[0] = this->checkOnOff();
    webClientSendBytes[1] = this->inputRegisterValues[ROOM_TEMP] & 0xFF;
    webClientSendBytes[2] = (this->inputRegisterValues[ROOM_TEMP] >> 8) & 0xFF;
    webClientSendBytes[3] = this->inputRegisterValues[INPUT_REGISTER_SET_TEMP] & 0xFF;
    webClientSendBytes[4] = (this->inputRegisterValues[INPUT_REGISTER_SET_TEMP] >> 8) & 0xFF;
    webClientSendBytes[5] = this->inputRegisterValues[OPERATION_MODE];
    webClientSendBytes[6] = this->inputRegisterValues[MANUAL_MODE_FAN_SPEED];
  }

  // ===== debug =====
  void debug() {
    if (!mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, HOLDING_REGISTERS_START_ADDRESS, HOLDING_REGISTER_COUNT)) {
      Serial.println(mbClient.lastError());
    } else {
      for (uint16_t i = 0; i < HOLDING_REGISTER_COUNT; i++) {
        this->holdingRegisterValues[i] = mbClient.read();
      }
    }

    if (!mbClient.requestFrom(this->slaveId, INPUT_REGISTERS, INPUT_REGISTERS_START_ADDRESS, INPUT_REGISTER_VALUES_COUNT)) {
      Serial.println(mbClient.lastError());
    } else {
      for (uint16_t i = 0; i < INPUT_REGISTER_VALUES_COUNT; i++) {
        this->inputRegisterValues[i] = mbClient.read();
      }
    }

    for (uint16_t i = 0; i < HOLDING_REGISTER_COUNT; i++) {
      Serial.print(this->holdingRegisterValues[i]);
      Serial.print(", ");
    }
    Serial.println();

    for (uint16_t i = 0; i < INPUT_REGISTER_VALUES_COUNT; i++) {
      Serial.print(this->inputRegisterValues[i]);
      Serial.print(", ");
    }
    Serial.println();

    Serial.println(this->inputRegisterValues[OPERATION_MODE]);
    Serial.println(this->inputRegisterValues[ROOM_TEMP]);
    Serial.println(this->inputRegisterValues[INPUT_REGISTER_SET_TEMP]);
    Serial.println(this->inputRegisterValues[MANUAL_MODE_FAN_SPEED]);
    Serial.println(this->checkOnOff());
  }

  ~ModbusBroker(){};
};

#endif