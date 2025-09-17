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
  uint8_t aiMappingMode;

  byte DIPayload = 0;
  byte DOPayload = 0;

  unsigned long prevMillisDisplay;


public:
  MainSystem(DigitalInput *digitalInputs, DigitalOutput *digitalOutputs, AnalogInput *analogInputs, AnalogOutput *analogOutputs, uint8_t aiMappingMode)
    : digitalInputs(digitalInputs), digitalOutputs(digitalOutputs), analogInputs(analogInputs), analogOutputs(analogOutputs), aiMappingMode(aiMappingMode) {
  }

  void setup() {
    // AIPayload.reserve(96);
    // AOPayload.reserve(32);
  }

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
      displayClient.prepareBuffer(nbiot.connState, nbiotCSQ.toInt(), this->DIPayload, this->DOPayload, analogInputs, analogOutputs, aiMappingMode);
      displayClient.sendBuffer();
      this->prevMillisDisplay = millis();
    }
  }

  void handlePublishContent() {
    /*=== 1. build the payload ===*/

    /*=== DI ===*/
    this->DIPayload = 0;
    for (size_t i = 0; i < DI_NUMS; i++) {
      // this->DIPayload |= digitalInputs[i].getState() << i;
      this->DIPayload = 255;
    }

    /*=== DO ===*/
    this->DOPayload = 0;
    for (size_t i = 0; i < DO_NUMS; i++) {
      // this->DOPayload |= digitalOutputs[i].getState() << i;
      this->DOPayload = 255;
    }

    /*=== AI ===*/
    AIPayload = "[";
    for (size_t i = 0; i < AI_NUMS; i++) {
      // AIPayload += analogInputs[i].getValue();
      AIPayload += 9999;
      if (i < AI_NUMS - 1) {
        AIPayload += ",";
      }
    }
    AIPayload += "]";

    /*=== AO ===*/
    AOPayload = "[";
    for (size_t i = 0; i < AO_NUMS; i++) {
      // AOPayload += analogOutputs[i].getValue();
      AOPayload += 255;
      if (i < AO_NUMS - 1) {
        AOPayload += ",";
      }
    }
    AOPayload += "]";

    /*=== 2. prepare the msg to be published ===*/
    if (!nbiot.pubMsgPayloadLock) {
      nbiotPubMsgPayload = "{\"csq\":";
      nbiotPubMsgPayload.concat(nbiotCSQ);
      nbiotPubMsgPayload.concat(",");
      nbiotPubMsgPayload.concat("\"din\":");
      nbiotPubMsgPayload.concat(this->DIPayload);
      nbiotPubMsgPayload.concat(",");
      nbiotPubMsgPayload.concat("\"dout\":");
      nbiotPubMsgPayload.concat(this->DOPayload);
      nbiotPubMsgPayload.concat(",");
      nbiotPubMsgPayload.concat("\"ain\":");
      nbiotPubMsgPayload.concat(AIPayload);
      nbiotPubMsgPayload.concat(",");
      nbiotPubMsgPayload.concat("\"aout\":");
      nbiotPubMsgPayload.concat(AOPayload);
      nbiotPubMsgPayload.concat("}");
    }

    nbiotPubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    nbiotPubMsgPrepare.concat(nbiotIMEI);
    nbiotPubMsgPrepare.concat("/in,");
    nbiotPubMsgPrepare.concat(nbiotPubMsgPayload.length());

    nbiotPubMsgCommand = nbiotPubMsgPrepare;
    nbiotPubMsgCommand.concat(",");
    nbiotPubMsgCommand.concat(nbiotPubMsgPayload);
  }

  void handleSubscribeContent() {
    if (nbiotSubMsgContent.length() <= 0) {
      return;
    }

    /*=== DO/AO Control ===*/
    int idx = -1;

    /*=== DO Control ===*/
    idx = nbiotSubMsgContent.indexOf("D");
    
    Serial.println(nbiotSubMsgContent);
    Serial.println("recv:" + idx);
    Serial.println(nbiotSubMsgContent.charAt(1));

    if (idx > -1) {
      char c1 = nbiotSubMsgContent.charAt(1);
      char c2 = nbiotSubMsgContent[2];
      char c3 = nbiotSubMsgContent[3];
      char c4 = nbiotSubMsgContent[4];

      if ("1" <= c1 && c1 <= "8") {
        /*=== DO Single Control ===*/
        if (c3 == "0") {
          digitalOutputs[atoi(c1) - 1].connect();
        } else {
          digitalOutputs[atoi(c1) - 1].cut();
        }
      } else if (c1 == ":") {
        /*=== DO Bulk Control ===*/
        byte b1 = utils.hexCharToByte(c3);
        byte b2 = utils.hexCharToByte(c4);
        byte finalByte = (b1 << 4) | b2;

        for (int i = 7; i >= 0; i--) {
          if (bitRead(finalByte, i) == 1) {
            digitalOutputs[7 - i].connect();
          } else {
            digitalOutputs[7 - i].cut();
          }
        }
      }
    }

    /*=== AO Control ===*/
    idx = nbiotSubMsgContent.indexOf("A");
    if (idx > -1) {
      char c1 = nbiotSubMsgContent[1];
      char c2 = nbiotSubMsgContent[2];
      char c3 = nbiotSubMsgContent[3];
      char c4 = nbiotSubMsgContent[4];

      byte b1 = utils.hexCharToByte(c3);
      byte b2 = utils.hexCharToByte(c4);
      byte finalByte = (b1 << 4) | b2;

      analogOutputs[atoi(c1) - 1].set(finalByte);
    }

    nbiotSubMsgContent = "";
  }

  ~MainSystem() {}
};

#endif