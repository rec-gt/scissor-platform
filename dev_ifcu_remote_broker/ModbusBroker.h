#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "AutoTimer.h"
#include "Globals.h"

#ifndef ModbusBroker_H
#define ModbusBroker_H

RS485Class rs485(Serial3, 2, 3, 4);
ModbusRTUClientClass mbClient(rs485);

AutoTimer timer;

class ModbusBroker {


private:
  enum Status {
    MB_WRITE,
    MB_READ,
  };

  Status currStatus;

  void handleReadData() {
    if (!mbClient.requestFrom(SLAVE_ID, INPUT_REGISTERS, 30000, 7)) {
      Serial.println(mbClient.lastError());
    } else {
      for (uint16_t i = 0; i < 7; i++) {
        responseValues[i] = mbClient.read();
      }
    }
  }

  void handleWrite4x(unsigned int addr, unsigned int value) {
    if (!mbClient.holdingRegisterWrite(SLAVE_ID, addr, value)) {
      Serial.println(mbClient.lastError());
    } else {
      Serial.println("Response OK");
    }
  }

  void handleWriteData() {
    this->handleWrite4x(40000, requestValues[0]);  // on/off
    this->handleWrite4x(40004, requestValues[1]);  // set point temp
    this->handleWrite4x(40002, requestValues[2]);  // mode
    this->handleWrite4x(40003, requestValues[3]);  // fan speed
  }

public:
  ModbusBroker(){};

  void loop() {
    if (this->currStatus == MB_WRITE) {
      if (timer.autoExpire(500)) {
        this->handleWriteData();
        this->currStatus == MB_READ;
      }
    } else if (this->currStatus == MB_READ) {
      if (timer.autoExpire(500)) {
        this->handleReadData();
        this->currStatus == MB_WRITE;
      }
    }
  }

  ~ModbusBroker(){};
};

#endif