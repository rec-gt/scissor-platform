
#include "AutoTimer.h"
#include "Globals.h"

#ifndef ModbusBroker_H
#define ModbusBroker_H

AutoTimer timer;

class ModbusBroker {
private:
  enum Status {
    MB_WRITE,
    MB_READ,
  };

  Status currStatus;

  void handleReadData() {
    if (!mbClient.requestFrom(SLAVE_ID, INPUT_REGISTERS, 30000UL, 10)) {
      Serial.println(mbClient.lastError());
    } else {
      Serial.print(mbClient.read());
      Serial.println();
      Serial.print(mbClient.read());
      Serial.println();
      Serial.print(mbClient.read());
      Serial.println();
      Serial.print(mbClient.read());
      Serial.println();
      Serial.print(mbClient.read());
      Serial.println();
      Serial.print(mbClient.read());
      Serial.println();
      Serial.print(mbClient.read());
      Serial.println();
      Serial.print(mbClient.read());
      Serial.println();
      Serial.print(mbClient.read());
      Serial.println();
      Serial.print(mbClient.read());  // maximum adj. set temp
      Serial.println();
      Serial.print(mbClient.read());  // minimum adj. set temp
      Serial.println();



      // for (uint16_t i = 0; i < 11; i++) {
      //   Serial.print(responseValues[i]);
      //   Serial.print(", ");
      // }
      // Serial.println();
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
      // if (timer.autoExpire(500)) {
      //   this->handleWriteData();
      //   this->currStatus = MB_READ;
      // }

      this->currStatus = MB_READ;
    } else if (this->currStatus == MB_READ) {
      this->handleReadData();
      this->currStatus = MB_WRITE;
    }
  }

  ~ModbusBroker(){};
};

#endif