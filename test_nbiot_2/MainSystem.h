#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "Globals.h"
#include "NBIoT.h"
#include "DisplayClient.h"

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
  String AIPayload = "";
  String AOPayload = "";

  unsigned long prevMillisDisplay;

  String subsMsg = "";

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
      displayClient.prepareBuffer(nbiot.connState, nbiot.CSQ.toInt(), this->DIPayload, this->DOPayload, analogInputs, analogOutputs, aiMappingMode);
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
    }

    /*=== DO ===*/
    this->DOPayload = 0;
    for (size_t i = 0; i < DO_NUMS; i++) {
      this->DOPayload |= digitalOutputs[i].getState() << i;
    }

    /*=== AI ===*/
    this->AIPayload = "[";
    for (size_t i = 0; i < AI_NUMS; i++) {
      this->AIPayload += analogInputs[i].getValue();
      if (i < AI_NUMS - 1) {
        this->AIPayload += ",";
      }
    }
    this->AIPayload += "]";

    /*=== AO ===*/
    this->AOPayload = "[";
    for (size_t i = 0; i < AO_NUMS; i++) {
      this->AOPayload += analogOutputs[i].getValue();
      if (i < AO_NUMS - 1) {
        this->AOPayload += ",";
      }
    }
    this->AOPayload += "]";

    /*=== 2. prepare the msg to be published ===*/
    nbiot.pubMsgPayload = "{\"csq\":";
    nbiot.pubMsgPayload.concat(nbiot.CSQ);
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"cgatt\":");
    nbiot.pubMsgPayload.concat(nbiot.CGATT);
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"cereg\":\"");
    nbiot.pubMsgPayload.concat(nbiot.CEREG);
    nbiot.pubMsgPayload.concat("\"");
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"din\":");
    nbiot.pubMsgPayload.concat(String(this->DIPayload));
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"dout\":");
    nbiot.pubMsgPayload.concat(String(this->DOPayload));
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"ain\":");
    nbiot.pubMsgPayload.concat(this->AIPayload);
    nbiot.pubMsgPayload.concat(",");
    nbiot.pubMsgPayload.concat("\"current\":");
    nbiot.pubMsgPayload.concat(this->AOPayload);
    nbiot.pubMsgPayload.concat("}");

    nbiot.pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    nbiot.pubMsgPrepare.concat(nbiot.IMEI);
    nbiot.pubMsgPrepare.concat("/in,");
    nbiot.pubMsgPrepare.concat(String(nbiot.pubMsgPayload.length()));

    nbiot.pubMsgCommand = nbiot.pubMsgPrepare;
    nbiot.pubMsgCommand.concat(",");
    nbiot.pubMsgCommand.concat(nbiot.pubMsgPayload);
  }

  void handleSubscribeContent() {

    nbiot.readRecvMsg(subsMsg);

    if (subsMsg.length() <= 0) {
      return;
    }

    Serial.println(subsMsg);
  }

  ~MainSystem() {}
};

#endif