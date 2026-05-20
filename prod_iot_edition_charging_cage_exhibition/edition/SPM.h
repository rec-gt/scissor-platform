#include "./SubGlobals.h"
Timer spmTimer(10000UL);

#ifndef SPM_H
#define SPM_H

class SPM {
private:
  byte slaveId;

  float IEEEfloat(uint32_t val) {
    union {
      uint32_t i;
      float f;
    } u;
    u.i = val;
    return u.f;
  }

public:
  SPM(byte slaveId)
    : slaveId(slaveId){};

  bool init() {
    if (!mbRtuClient.begin(9600, SERIAL_8N1)) {
      Serial.println(F("Failed to start Modbus RTU Client!"));
      while (1) {};
    }
    return false;
  }

  void loop() {
    if (spmTimer.autoTimeout(1000)) {
      this->readIn1000ms();
      this->showData();
    }
  }

  void readIn1000ms() {
    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 1006, 2);
    holdingRegisterValues[0] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
  }

  void showData() {
    Serial.println(F("\r\n>>> Print Data\r\n"));

    for (byte i = 0; i < PARAMETERS_SIZE; i++) {
      Serial.print(holdingRegisterDescription[i]);
      Serial.print(F(": "));
      Serial.println(holdingRegisterValues[i], 4);
    }
  }
};

#endif
