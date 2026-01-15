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
      Serial.println("Failed to start Modbus RTU Client!");
      while (1) {};
    }
  }

  void loop() {
    if (deviceTimer.autoTimeout(1000)) {
      this->readIn1000ms();
      // this->handlePublishContent();
      this->showData();
    }
  }

  void readIn1000ms() {
    mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 0, 10);

    for (size_t i = 0; i < PARAMETERS_SIZE; i++) {
      holdingRegisterValues[i] = (uint32_t)mbRtuClient.read();
    }
  }

  void showData() {
    Serial.println(F("\r\n>>> Print Data\r\n"));

    for (byte i = 0; i < PARAMETERS_SIZE; i++) {
      Serial.print(holdingRegisterDescription[i]);
      Serial.print(": ");
      Serial.println((uint32_t)holdingRegisterValues[i]);
    }

    if (mbRtuClient.lastError()) {
      Serial.println(mbRtuClient.lastError());
    }
  }

  void handlePublishContent() {
    // if (!nbiot.pubMsgPayloadLock) {
    //   nbiotPubMsgPayload = F("{\"csq\":");
    //   nbiotPubMsgPayload.concat(nbiotCSQ);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(F("\"ain\":"));
    //   nbiotPubMsgPayload.concat(F("["));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UAN]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UBN]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UCN]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UAB]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UBC]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_UCA]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IA]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IB]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IC]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_P_TOTAL]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_PF_TOTAL]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IN_CALCULATED]);
    //   nbiotPubMsgPayload.concat(F("]"));
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(F("\"aout\":"));
    //   nbiotPubMsgPayload.concat(F("["));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_KWH_TOTAL]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_KWH_TOTAL] * 0.7);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IB_THD]);
    //   nbiotPubMsgPayload.concat(F(","));
    //   nbiotPubMsgPayload.concat(holdingRegisterValues[HR_IC_THD]);
    //   nbiotPubMsgPayload.concat(F("]"));
    //   nbiotPubMsgPayload.concat(F("}"));
    // }

    // nbiotPubMsgPrepare = F("AT+QMTPUB=0,0,0,0,rgt/");
    // nbiotPubMsgPrepare.concat(nbiotIMEI);
    // nbiotPubMsgPrepare.concat(F("/in,"));
    // nbiotPubMsgPrepare.concat(nbiotPubMsgPayload.length());

    // nbiotPubMsgCommand = nbiotPubMsgPrepare;
    // nbiotPubMsgCommand.concat(F(","));
    // nbiotPubMsgCommand.concat(nbiotPubMsgPayload);
  }
};

#endif
