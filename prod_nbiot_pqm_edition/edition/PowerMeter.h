#include "./SubGlobals.h"

#ifndef PowerMeter_H
#define PowerMeter_H

class PowerMeter {
private:
  byte slaveId;
  unsigned long prevMillis = millis();

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

  void loop() {
    if (millis() - this->prevMillis >= 1000) {
      this->readIn1000ms();
      this->handlePublishContent();
      this->showData();

      this->prevMillis = millis();
    }
  }

  void readIn1000ms() {
    mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 0, 6);
    holdingRegisterValues[HR_UAN] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
    holdingRegisterValues[HR_UBN] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
    holdingRegisterValues[HR_UCN] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

    delay(15);

    mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 8, 6);
    holdingRegisterValues[HR_UAB] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
    holdingRegisterValues[HR_UBC] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
    holdingRegisterValues[HR_UCA] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

    delay(15);

    mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 16, 6);
    holdingRegisterValues[HR_IA] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
    holdingRegisterValues[HR_IB] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
    holdingRegisterValues[HR_IC] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

    delay(15);

    mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 30, 2);
    holdingRegisterValues[HR_P_TOTAL] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

    delay(15);

    mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 54, 2);
    holdingRegisterValues[HR_PF_TOTAL] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

    delay(15);

    mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 70, 2);
    holdingRegisterValues[HR_IN_CALCULATED] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

    delay(15);

    mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 506, 2);
    holdingRegisterValues[HR_KWH_TOTAL] = (((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read()) * 0.1;

    delay(15);

    mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 1400, 6);
    holdingRegisterValues[HR_IA_THD] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
    holdingRegisterValues[HR_IB_THD] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());
    holdingRegisterValues[HR_IC_THD] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

    delay(15);

    mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 3434, 2);
    holdingRegisterValues[HR_S_TOTAL] = IEEEfloat(((uint32_t)mbClient.read() << 16) | (uint32_t)mbClient.read());

    delay(15);

    Serial.println(mbClient.lastError());

    Serial.println();
  }

  void showData() {
    for (byte i = 0; i < PARAMETERS_SIZE; i++) {
      Serial.print(holdingRegisterDescription[i]);
      Serial.print(": ");
      Serial.println(holdingRegisterValues[i]);
    }
  }

  void handlePublishContent() {
    if (!nbiot.pubMsgPayloadLock) {
      nbiotPubMsgPayload = F("{\"csq\":");
      nbiotPubMsgPayload.concat(nbiotCSQ);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(F("\"ain\":"));
      nbiotPubMsgPayload.concat(F("["));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UAN]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UBN]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UCN]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UAB]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UBC]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UCA]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IA]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IB]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IC]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_P_TOTAL]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_PF_TOTAL]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IN_CALCULATED]);
      nbiotPubMsgPayload.concat(F("]"));
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(F("\"aout\":"));
      nbiotPubMsgPayload.concat(F("["));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_KWH_TOTAL]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_KWH_TOTAL] * 0.7);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IB_THD]);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IC_THD]);
      nbiotPubMsgPayload.concat(F("]"));
      nbiotPubMsgPayload.concat(F("}"));
    }

    nbiotPubMsgPrepare = F("AT+QMTPUB=0,0,0,0,rgt/");
    nbiotPubMsgPrepare.concat(nbiotIMEI);
    nbiotPubMsgPrepare.concat(F("/in,"));
    nbiotPubMsgPrepare.concat(nbiotPubMsgPayload.length());

    nbiotPubMsgCommand = nbiotPubMsgPrepare;
    nbiotPubMsgCommand.concat(F(","));
    nbiotPubMsgCommand.concat(nbiotPubMsgPayload);
  }
};

#endif
