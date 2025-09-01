
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
    if (!mbClient.requestFrom(SLAVE_ID, INPUT_REGISTERS, 30000UL, 11)) {
      Serial.println(mbClient.lastError());
    } else {
      mbClient.read();                                            // ignore
      responseValues[0] = ((mbClient.read() & 0b01000000) != 0);  // on/off
      mbClient.read();                                            // ignore
      responseValues[1] = mbClient.read();                        // Mode
      responseValues[2] = mbClient.read();                        // Fan Speed
      responseValues[3] = mbClient.read();                        // Room Temp
      responseValues[4] = mbClient.read();                        // Set-point Temp
      mbClient.read();                                            // ignore
      mbClient.read();                                            // ignore
      responseValues[5] = mbClient.read();                        // maximum adj. set temp
      responseValues[6] = mbClient.read();                        // minimum adj. set temp

      for (uint16_t i = 0; i < 7; i++) {
        Serial.print(responseValues[i]);
        Serial.print(", ");
      }
      Serial.println();
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