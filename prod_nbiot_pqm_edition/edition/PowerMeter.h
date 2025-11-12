#include "./SubGlobals.h"

#ifndef PowerMeter_H
#define PowerMeter_H

class PowerMeter {
private:
  byte slaveId;
  unsigned long prevMillis = millis();

  char* holdingRegisterDescription[PARAMETERS_SIZE] = {};


  float IEEEfloat(uint32_t uint32) {
    union {
      uint32_t i;
      float f;
    } u;
    u.i = uint32;
    return u.f;
  }

public:
  PowerMeter(byte slaveId)
    : slaveId(slaveId){};

  void init() {
    this->holdingRegisterDescription[HR_UAN] = (char*)F("Uan             ");
    this->holdingRegisterDescription[HR_UBN] = (char*)F("Ubn             ");
    this->holdingRegisterDescription[HR_UCN] = (char*)F("Ucn             ");
    this->holdingRegisterDescription[HR_UAB] = (char*)F("Uab             ");
    this->holdingRegisterDescription[HR_UBC] = (char*)F("Ubc             ");
    this->holdingRegisterDescription[HR_UCA] = (char*)F("Uca             ");
    this->holdingRegisterDescription[HR_IA] = (char*)F("Ia              ");
    this->holdingRegisterDescription[HR_IB] = (char*)F("Ib              ");
    this->holdingRegisterDescription[HR_IC] = (char*)F("Ic              ");
    this->holdingRegisterDescription[HR_P_TOTAL] = (char*)F("P Total         ");
    this->holdingRegisterDescription[HR_PF_TOTAL] = (char*)F("PF Total        ");
    this->holdingRegisterDescription[HR_IN_CALCULATED] = (char*)F("In (Calculated) ");
    this->holdingRegisterDescription[HR_KWH_TOTAL] = (char*)F("kWh Total       ");
    this->holdingRegisterDescription[HR_IA_THD] = (char*)F("Ia THD          ");
    this->holdingRegisterDescription[HR_IB_THD] = (char*)F("Ib THD          ");
    this->holdingRegisterDescription[HR_IC_THD] = (char*)F("Ic THD          ");
    this->holdingRegisterDescription[HR_S_TOTAL] = (char*)F("S Total         ");
  }

  void loop() {
    if (millis() - this->prevMillis >= 1000) {
      this->readIn1000ms();
      this->handlePublishContent();
      this->showData();

      this->prevMillis = millis();
    }
  }

  void readIn1000ms() {
    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 0, 6);
    holdingRegisterValues[HR_UAN] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());
    holdingRegisterValues[HR_UBN] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());
    holdingRegisterValues[HR_UCN] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());

    delay(15);

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 8, 6);
    holdingRegisterValues[HR_UAB] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());
    holdingRegisterValues[HR_UBC] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());
    holdingRegisterValues[HR_UCA] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());

    delay(15);

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 16, 6);
    holdingRegisterValues[HR_IA] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());
    holdingRegisterValues[HR_IB] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());
    holdingRegisterValues[HR_IC] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());

    delay(15);

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 30, 2);
    holdingRegisterValues[HR_P_TOTAL] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());

    delay(15);

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 54, 2);
    holdingRegisterValues[HR_PF_TOTAL] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());

    delay(15);

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 70, 2);
    holdingRegisterValues[HR_IN_CALCULATED] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());

    delay(15);

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 506, 2);
    holdingRegisterValues[HR_KWH_TOTAL] = (((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read()) * 0.1;

    delay(15);

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 1400, 6);
    holdingRegisterValues[HR_IA_THD] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());
    holdingRegisterValues[HR_IB_THD] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());
    holdingRegisterValues[HR_IC_THD] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());

    delay(15);

    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 3434, 2);
    holdingRegisterValues[HR_S_TOTAL] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | (uint32_t)mbRtuClient.read());

    delay(15);

    Serial.println(mbRtuClient.lastError());

    Serial.println();
  }

  void showData() {
    for (byte i = 0; i < PARAMETERS_SIZE; i++) {
      Serial.print(holdingRegisterDescription[i]);
      Serial.print(F(": "));
      Serial.println(holdingRegisterValues[i]);
    }
  }

};

#endif
