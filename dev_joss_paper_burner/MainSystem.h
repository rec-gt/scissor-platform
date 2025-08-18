#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "Globals.h"

#ifndef MAINSYSTEM_H
#define MAINSYSTEM_H

class MainSystem {
private:
  DigitalInput *digitalInputs;
  DigitalOutput *digitalOutputs;
  AnalogInput *analogInputs;
  AnalogOutput *analogOutputs;

  byte DIPayload = 0;
  byte DOPayload = 0;
  unsigned long AIPayload[AI_NUMS] = {};
  unsigned long AOPayload[AO_NUMS] = {};

public:
  MainSystem(DigitalInput *digitalInputs, DigitalOutput *digitalOutputs, AnalogInput *analogInputs, AnalogOutput *analogOutputs) {
    this->digitalInputs = digitalInputs;
    this->digitalOutputs = digitalOutputs;
    this->analogInputs = analogInputs;
    this->analogOutputs = analogOutputs;
  }

  void listen() {
    for (size_t i = 0; i < DI_NUMS; i++) {
      digitalInputs[i].listen();
    }

    for (size_t i = 0; i < AI_NUMS; i++) {
      analogInputs[i].listen();
    }
  }

  void buildPayloads() {
    this->DIPayload = 0;
    for (size_t i = 0; i < DI_NUMS; i++) {
      this->DIPayload |= digitalInputs[i].getState() << i;
    }

    this->DOPayload = 0;
    for (size_t i = 0; i < DO_NUMS; i++) {
      this->DOPayload |= digitalOutputs[i].getState() << i;
    }
    Serial.print(this->DIPayload);
    Serial.print(" | ");
    Serial.print(this->DOPayload);
    Serial.println();
  }

  void preparePubMsg() {
    this->buildPayloads();

    pubMsgContent = "{\"csq\":";
    pubMsgContent.concat("nbiot.CSQ");
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"cgatt\":");
    pubMsgContent.concat("nbiot.CGATT");
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"cereg\":\"");
    pubMsgContent.concat("nbiot.CEREG");
    pubMsgContent.concat("\"");
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"din\":");
    pubMsgContent.concat("255");
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"dout\":");
    pubMsgContent.concat("255");
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"ain\":");
    pubMsgContent.concat("[1,2,3,4,5,6,7,8]");
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"current\":");
    pubMsgContent.concat("[1,2,3,4,5,6,7,8]");
    pubMsgContent.concat("}");

    int contentLen = pubMsgContent.length();

    pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    pubMsgPrepare.concat("nbiot.IMEI");
    pubMsgPrepare.concat("/in,");
    pubMsgPrepare.concat(String(contentLen));

    pubMsgForce = pubMsgPrepare;
    pubMsgForce.concat(",");
    pubMsgForce.concat(pubMsgContent);
  }

  ~MainSystem() {}
};

#endif