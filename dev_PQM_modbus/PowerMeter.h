#include "Globals.h"
#include "NBIoT.h"

#ifndef PowerMeter_H
#define PowerMeter_H

class PowerMeter {
private:
  byte slaveId;
  unsigned long prevMillis = millis();
  static constexpr uint16_t PARAMETERS_SIZE = 17;

  enum holdingRegisterEnums[PARAMETERS_SIZE] = {
    HR_UAN,
    HR_UBN,
    HR_UCN,
    HR_UAB,
    HR_UBC,
    HR_UCA,
    HR_IA,
    HR_IB,
    HR_IC,
    HR_P_TOTAL,
    HR_PF_TOTAL,
    HR_IN_CALCULATED,
    HR_KWH_TOTAL,
    HR_IA_THD,
    HR_IB_THD,
    HR_IC_THD,
    HR_S_TOTAL
  };

  float holdingRegisterValues[PARAMETERS_SIZE] = {};

  char* holdingRegisterDescription[PARAMETERS_SIZE] = {
    (char*)"Uan             ",
    (char*)"Ubn             ",
    (char*)"Ucn             ",
    (char*)"Uab             ",
    (char*)"Ubc             ",
    (char*)"Uca             ",
    (char*)"Ia              ",
    (char*)"Ib              ",
    (char*)"Ic              ",
    (char*)"P Total         ",
    (char*)"PF Total        ",
    (char*)"In (Calculated) ",
    (char*)"kWh Total       ",
    (char*)"Ia THD          ",
    (char*)"Ib THD          ",
    (char*)"Ic THD          ",
    (char*)"S Total         ",
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
  PowerMeter(byte slaveId)
    : slaveId(slaveId){};

  void read() {
    if (millis() - this->prevMillis >= 1000) {
      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 0, 6);
      holdingRegisterValues[HR_UAN] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
      holdingRegisterValues[HR_UBN] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
      holdingRegisterValues[HR_UCN] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

      delay(3);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 8, 6);
      holdingRegisterValues[HR_UAB] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
      holdingRegisterValues[HR_UBC] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
      holdingRegisterValues[HR_UCA] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

      delay(3);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 16, 6);
      holdingRegisterValues[HR_IA] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
      holdingRegisterValues[HR_IB] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
      holdingRegisterValues[HR_IC] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

      delay(3);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 30, 2);
      holdingRegisterValues[HR_P_TOTAL] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

      delay(3);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 54, 2);
      holdingRegisterValues[HR_PF_TOTAL] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

      delay(3);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 70, 2);
      holdingRegisterValues[HR_IN_CALCULATED] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

      delay(3);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 506, 2);
      Serial.print(mbClient.read());
      // holdingRegisterValues[HR_KWH_TOTAL] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

      delay(3);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 1400, 6);
      holdingRegisterValues[HR_IA_THD] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
      holdingRegisterValues[HR_IB_THD] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
      holdingRegisterValues[HR_IC_THD] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

      delay(3);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 3430, 6);
      mbClient.read();
      mbClient.read();
      mbClient.read();
      mbClient.read();
      holdingRegisterValues[HR_S_TOTAL] = ((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read();

      delay(3);

      Serial.println(mbClient.lastError());

      Serial.println();

      for (byte i = 0; i < PARAMETERS_SIZE; i++) {
        Serial.print(holdingRegisterDescription[i]);
        Serial.print(": ");
        Serial.println(holdingRegisterValues[i]);
      }

      this->prevMillis = millis();
    }
  }
};

#endif
