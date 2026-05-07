#include "./SubGlobals.h"

#ifndef CET_H
#define CET_H

#define PARAMETERS_SIZE 10

class CET {
private:
  byte slaveId;

  float holdingRegisterValues[PARAMETERS_SIZE];

  char* holdingRegisterDescription[PARAMETERS_SIZE] = {
    (char*)"HR_01 ",
    (char*)"HR_02 ",
    (char*)"HR_03 ",
    (char*)"HR_04 ",
    (char*)"HR_05 ",
    (char*)"HR_06 ",
    (char*)"HR_07 ",
    (char*)"HR_08 ",
    (char*)"HR_09 ",
    (char*)"HR_10 ",
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
    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 9820, 1);

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      holdingRegisterValues[i] = (uint32_t)mbRtuClient.read();
    }

    if (mbRtuClient.lastError()) {
      modbusCounter.accu();
    }

    if (modbusCounter.over(3)) {
      // reconnect modbus
      mbRtuClient.end();
      mbRtuClient.begin(9600);
    }
  }

  void showData() {
    Serial.println(F("\r\n>>> Print Data\r\n"));

    for (byte i = 0; i < PARAMETERS_SIZE; i++) {
      Serial.print(holdingRegisterDescription[i]);
      Serial.print(F(": "));
      Serial.println((uint32_t)holdingRegisterValues[i]);
    }

    if (mbRtuClient.lastError()) {
      Serial.println(mbRtuClient.lastError());
    }
  }
};

#endif
