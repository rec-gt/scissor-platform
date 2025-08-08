#include "Globals.h"

#ifndef IFCU_H
#define IFCU_H

class IFCU {
private:
  byte slaveId;

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

  void on() {
    this->handleWrite4x(40000, 1);
  }

  void off() {
    this->handleWrite4x(40000, 0);
  }
};

#endif
