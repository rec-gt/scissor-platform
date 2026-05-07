#include "./SubGlobals.h"

#ifndef CET_H
#define CET_H

#define PARAMETERS_SIZE 4

class CET {
private:
  byte slaveId;

  float holdingRegisterValues[PARAMETERS_SIZE];

  char* holdingRegisterDescription[PARAMETERS_SIZE] = {
    (char*)"V",
    (char*)"I",
    (char*)"kW",
    (char*)"kW Total",
  };

  float IEEEfloat(uint32_t uint32) {
    union {
      uint32_t i;
      float f;
    } u;
    u.i = uint32;
    return u.f;
  }

public:
  CET(byte slaveId)
    : slaveId(slaveId){};

  void init() {
    if (!mbRtuClient.begin(9600)) {
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
    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 0, 10);

    mbRtuClient.read();  // ignore
    holdingRegisterValues[0] = (uint32_t)mbRtuClient.read();
    mbRtuClient.read();  // ignore
    holdingRegisterValues[1] = (uint32_t)mbRtuClient.read();
    mbRtuClient.read();  // ignore
    holdingRegisterValues[2] = (uint32_t)mbRtuClient.read();

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 101, 1);
    holdingRegisterValues[3] = (uint32_t)mbRtuClient.read();
  }

  void showData() {
    Serial.println(F("\r\n>>> Print Data\r\n"));

    for (byte i = 0; i < PARAMETERS_SIZE; i++) {
      Serial.print(holdingRegisterDescription[i]);
      Serial.print(F(": "));
      Serial.println((uint32_t)holdingRegisterValues[i]);
    }
  }
};

#endif
