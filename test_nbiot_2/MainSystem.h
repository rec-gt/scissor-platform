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
      this->DIPayload |= digitalInputs[i].getState() << i;
      // this->DIPayload = 255;
    }

    /*=== DO ===*/
    this->DOPayload = 0;
    for (size_t i = 0; i < DO_NUMS; i++) {
      this->DOPayload |= digitalOutputs[i].getState() << i;
      // this->DOPayload = 255;
    }

    /*=== AI ===*/
    AIPayload = F("[");
    for (size_t i = 0; i < AI_NUMS; i++) {
      // AIPayload += analogInputs[i].getValue();
      AIPayload += 4095;
      if (i < AI_NUMS - 1) {
        AIPayload += F(",");
      }
    }
    AIPayload += F("]");

    /*=== AO ===*/
    AOPayload = F("[");
    for (size_t i = 0; i < AO_NUMS; i++) {
      AOPayload += analogOutputs[i].getValue();
      // AOPayload += 255;
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
    Serial.print("idx:");
    Serial.println(idx);

    if (idx > -1) {
      char c1 = nbiotSubMsgContent.charAt(1);
      char c2 = nbiotSubMsgContent.charAt(2);
      char c3 = nbiotSubMsgContent.charAt(3);
      char c4 = nbiotSubMsgContent.charAt(4);


      if (1 <= (int)(c1 - '0') && (int)(c1 - '0') <= 8) {
        /*=== DO Single Control ===*/
        if (c3 == "0") {
          digitalOutputs[(int)(c1 - '0') - 1].cut();
        } else {
          digitalOutputs[(int)(c1 - '0') - 1].connect();
        }
      } else if ((int)(c1 - '0') == 10) {
        /*=== DO Bulk Control ===*/
        byte b1 = utils.hexCharToByte(c3);
        byte b2 = utils.hexCharToByte(c4);
        byte finalByte = (b1 << 4) | b2;
        Serial.print(finalByte);

        Serial.print(bitRead(finalByte, 0));
        Serial.print(bitRead(finalByte, 1));
        Serial.print(bitRead(finalByte, 2));
        Serial.print(bitRead(finalByte, 3));
        Serial.print(bitRead(finalByte, 4));
        Serial.print(bitRead(finalByte, 5));
        Serial.print(bitRead(finalByte, 6));
        Serial.print(bitRead(finalByte, 7));

        // for (int i = 7; i > 0; i--) {
        //   if (bitRead(finalByte, i) == 1) {
        //     Serial.print(7 - i);
        //     if (c2 == "0") {
        //       digitalOutputs[7 - i].cut();
        //     } else {
        //       digitalOutputs[7 - i].connect();
        //     }
        //   }
        // }
      }
      nbiotSubMsgContent = "";
      return;
    }

    /*=== AO Control ===*/
    idx = nbiotSubMsgContent.indexOf("A");

    Serial.println(nbiotSubMsgContent);
    Serial.print("idx:");
    Serial.println(idx);

    if (idx > -1) {
      char c1 = nbiotSubMsgContent.charAt(1);
      char c2 = nbiotSubMsgContent.charAt(2);
      char c3 = nbiotSubMsgContent.charAt(3);
      char c4 = nbiotSubMsgContent.charAt(4);

      byte b1 = utils.hexCharToByte(c3);
      byte b2 = utils.hexCharToByte(c4);
      byte finalByte = (b1 << 4) | b2;
      analogOutputs[(int)(c1 - '0') - 1].set(finalByte);

      nbiotSubMsgContent = "";
      return;
    }
  }

  ~MainSystem() {}
};

#endif