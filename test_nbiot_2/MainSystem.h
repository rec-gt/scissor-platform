#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "NBIoT.h"
#include "DisplayClient.h"
#include "Utils.h"
#include "Globals.h"

#ifndef MainSystem_H
#define MainSystem_H

class MainSystem {
private:
  DigitalInput *digitalInputs;
  DigitalOutput *digitalOutputs;
  AnalogInput *analogInputs;
  AnalogOutput *analogOutputs;

  byte DIPayload = 0;
  byte DOPayload = 0;

  unsigned long prevMillisDisplay;

public:
  MainSystem(DigitalInput *digitalInputs, DigitalOutput *digitalOutputs, AnalogInput *analogInputs, AnalogOutput *analogOutputs)
    : digitalInputs(digitalInputs), digitalOutputs(digitalOutputs), analogInputs(analogInputs), analogOutputs(analogOutputs) {}

  void loop() {
    /*=== Listen Input Ports ===*/
    this->listen();

    /*=== Display (for NBIoT, DO, AO, DI, AI)===*/
    this->handleDisplayContent();

    /*=== NBIoT Publish ===*/
    this->handlePublishContent();

    /*=== NBIoT Subscribe ===*/
    this->handleSubscribeContent();
  }

  void listen() {
    for (size_t i = 0; i < DI_NUMS; i++) {
      digitalInputs[i].listen();
    }

    for (size_t i = 0; i < AI_NUMS; i++) {
      analogInputs[i].listen();
    }
  }

  void handleDisplayContent() {
    if (millis() - this->prevMillisDisplay > 2000) {
      {
        int csq = nbiotCSQ.toInt();
        displayClient.prepareBuffer(nbiot.connState, csq, this->DIPayload, this->DOPayload, analogInputs, analogOutputs, 65535);
        displayClient.sendBuffer();
      }
      this->prevMillisDisplay = millis();
    }
  }

  void handlePublishContent() {
    /*=== 1. build the payload ===*/

    /*=== DI ===*/
    this->DIPayload = 0;
    for (size_t i = 0; i < DI_NUMS; i++) {
      this->DIPayload |= digitalInputs[i].getState() << i;
    }

    /*=== DO ===*/
    this->DOPayload = 0;
    for (size_t i = 0; i < DO_NUMS; i++) {
      this->DOPayload |= digitalOutputs[i].getState() << i;
    }

    /*=== AI ===*/
    AIPayload = F("[");
    for (size_t i = 0; i < AI_NUMS; i++) {
      AIPayload += analogInputs[i].getValue();
      if (i < AI_NUMS - 1) {
        AIPayload += F(",");
      }
    }
    AIPayload += F("]");

    /*=== AO ===*/
    AOPayload = F("[");
    for (size_t i = 0; i < AO_NUMS; i++) {
      AOPayload += analogOutputs[i].getValue();
      if (i < AO_NUMS - 1) {
        AOPayload += F(",");
      }
    }
    AOPayload += F("]");

    /*=== 2. prepare the msg to be published ===*/
    if (!nbiot.pubMsgPayloadLock) {
      nbiotPubMsgPayload = F("{\"csq\":");
      nbiotPubMsgPayload.concat(nbiotCSQ);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(F("\"din\":"));
      nbiotPubMsgPayload.concat(this->DIPayload);
      nbiotPubMsgPayload.concat(F(","));
      nbiotPubMsgPayload.concat(F("\"dout\":"));
      nbiotPubMsgPayload.concat(this->DOPayload);
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

  void handleSubscribeContent() {
    if (nbiotSubMsgContent.length() <= 0) {
      return;
    }

    byte b0 = nbiotSubMsgContent.charAt(0);
    byte b1 = nbiotSubMsgContent.charAt(1);
    byte b2 = nbiotSubMsgContent.charAt(2);
    byte b3 = nbiotSubMsgContent.charAt(3);
    byte b4 = nbiotSubMsgContent.charAt(4);

    if (b0 == 68) {                                                                   // D
      if (b1 == 58) {                                                                 // :
        byte finalByte = (utils.hexCharToByte(b3) << 4) | (utils.hexCharToByte(b4));  // hex -> dec -> byte
        bitRead(finalByte, 0) == 0 ? digitalOutputs[7].cut() : digitalOutputs[7].connect();
        bitRead(finalByte, 1) == 0 ? digitalOutputs[6].cut() : digitalOutputs[6].connect();
        bitRead(finalByte, 2) == 0 ? digitalOutputs[5].cut() : digitalOutputs[5].connect();
        bitRead(finalByte, 3) == 0 ? digitalOutputs[4].cut() : digitalOutputs[4].connect();
        bitRead(finalByte, 4) == 0 ? digitalOutputs[3].cut() : digitalOutputs[3].connect();
        bitRead(finalByte, 5) == 0 ? digitalOutputs[2].cut() : digitalOutputs[2].connect();
        bitRead(finalByte, 6) == 0 ? digitalOutputs[1].cut() : digitalOutputs[1].connect();
        bitRead(finalByte, 7) == 0 ? digitalOutputs[0].cut() : digitalOutputs[0].connect();
      } else if (49 <= b1 && b1 <= 56) {  // 1-8
        if (b3 == 48) {                   // 0
          digitalOutputs[b1 - 49].cut();
        } else {
          digitalOutputs[b1 - 49].connect();
        }
      }
    } else if (b0 == 65) {  // A
      if (49 <= b1 && b1 <= 53) {
        byte finalByte = (utils.hexCharToByte(b3) << 4) | utils.hexCharToByte(b4);  // hex -> dec -> byte
        analogOutputs[b1 - 49].set(finalByte);
      }
    }

    nbiotSubMsgContent = "";
  }

  ~MainSystem() {}
};

#endif