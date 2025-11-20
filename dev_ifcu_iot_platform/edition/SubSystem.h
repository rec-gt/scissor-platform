#ifndef SubSystem_H
#define SubSystem_H

#include "./SubGlobals.h"
#include "./SubModbus.h"
#include "../core/Globals.h"

SubModbus subModbus;

class SubSystem {
private:
  void buildPayloads() {
    /*=== AI ===*/
    AIPayload = F("[");
    for (size_t i = 0; i < 12; i++) {
      AIPayload += dataStorage[i];
      if (i < AI_NUMS - 1) {
        AIPayload += F(",");
      }
    }
    AIPayload += F("]");

    /*=== AO ===*/
    AOPayload = F("[");
    for (size_t i = 12; i < 16; i++) {
      AOPayload += dataStorage[i];
      if (i < AO_NUMS - 1) {
        AOPayload += F(",");
      }
    }
    AOPayload += F("]");
  }

  void handlePublishContent() {
    if (!nbiot.pubMsgPayloadLock) {
      nbiotPubMsgPayload = F("{\"csq\":");
      nbiotPubMsgPayload.concat(nbiotCSQ);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(F("\"din\":"));
      nbiotPubMsgPayload.concat(DIPayload);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(F("\"dout\":"));
      nbiotPubMsgPayload.concat(DOPayload);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(F("\"ain\":"));
      nbiotPubMsgPayload.concat(AIPayload);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(F("\"aout\":"));
      nbiotPubMsgPayload.concat(AOPayload);
      nbiotPubMsgPayload.concat(F("}"));
    }

    nbiotPubMsgPrepare = F("AT+QMTPUB=0,0,0,0,rgt/");
    nbiotPubMsgPrepare.concat(nbiotIMEI);
    nbiotPubMsgPrepare.concat(F("/in,"));
    nbiotPubMsgPrepare.concat(nbiotPubMsgPayload.length());

    nbiotPubMsgCommand = nbiotPubMsgPrepare;
    nbiotPubMsgCommand.concat(F(","));
    nbiotPubMsgCommand.concat(nbiotPubMsgPayload);

    // Serial.println(nbiotPubMsgPrepare);
    // Serial.println(nbiotPubMsgPayload);
    // Serial.println(nbiotPubMsgCommand);
  }

public:
  SubSystem(void) {}

  void init() {
    configAnalogInputResolution(0);
    subModbus.init();
  }

  void loop() {
    subModbus.loop();
    Serial.println(AOPayload);
    this->buildPayloads();
    this->handlePublishContent();
  }

  ~SubSystem() {}
};

#endif