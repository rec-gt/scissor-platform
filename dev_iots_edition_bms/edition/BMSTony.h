#include "./SubGlobals.h"

#ifndef BMSTony_H
#define BMSTony_H

class BMSTony {
private:
  byte slaveId;

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
  BMSTony(byte slaveId)
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
      // this->showData();
    }
    this->compare();
  }

  void readIn1000ms() {
    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 0, 10);

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      holdingRegisterValues[i] = (uint32_t)mbRtuClient.read();
    }
  }

  void compare() {
    bool areAllTheSame = true;
    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      if (holdingRegisterValues[i] != prevHoldingRegisterValues[i]) {
        prevHoldingRegisterValues[i] = holdingRegisterValues[i];  // update previous values
        areAllTheSame = false;
      }
    }

    if (!areAllTheSame) {
      Serial.println(F("Diff Detected!"));
      eventTriggerFlag = true;
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
