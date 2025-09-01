#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#ifndef ModbusBroker_H
#define ModbusBroker_H

#define SLAVE_ID 31

RS485Class rs485(Serial3, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

class ModbusBroker {
  enum Status {
    MB_WRITE,
    MB_READ,
  };

private:
  void handleRead3x() {
    if (millis() - this->prevMillis > 1000) {
      if (!mbClient.requestFrom(this->slaveId, INPUT_REGISTERS, INPUT_REGISTERS_START_ADDRESS, INPUT_REGISTER_VALUES_COUNT)) {
        Serial.println(mbClient.lastError());
      } else {
        for (uint16_t i = 0; i < INPUT_REGISTER_VALUES_COUNT; i++) {
          this->inputRegisterValues[i] = mbClient.read();
        }
      }

      this->prepareSendBuffer();

      this->prevMillis = millis();
    }
  }

  void handleWrite4x(unsigned int addr, unsigned int value) {
    if (!mbClient.holdingRegisterWrite(SLAVE_ID, addr, value)) {
      Serial.println(mbClient.lastError());
    } else {
      Serial.println("Response OK");
    }
  }

public:
  ModbusBroker(){};

  void handleWriteData() {
    this->handleWrite4x(40000, requestValues[0]);  // on/off
    this->handleWrite4x(40004, requestValues[1]);  // set point temp
    this->handleWrite4x(40002, requestValues[2]);  // mode
    this->handleWrite4x(40003, requestValues[3]);  // fan speed
  }

  ~ModbusBroker(){};
};

#endif