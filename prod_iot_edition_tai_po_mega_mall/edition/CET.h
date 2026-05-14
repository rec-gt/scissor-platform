#include "./SubGlobals.h"

#ifndef CET_H
#define CET_H

class CET {
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
  CET(byte slaveId)
    : slaveId(slaveId){};

  void init() {
    if (!mbRtuClient.begin(9600, SERIAL_8E1)) {
      Serial.println(F("Failed to start Modbus RTU Client!"));
      while (1) {};
    }
  }

  void loop() {
    if (deviceTimer.autoTimeout(1000)) {
      this->readIn1000ms();
      this->showData();
    }
  }

  void readIn1000ms() {
    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 0, 6);
    holdingRegisterValues[0] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
    holdingRegisterValues[1] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
    holdingRegisterValues[2] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 46, 2);
    holdingRegisterValues[3] = (((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read()) * 0.01;
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
