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

  byte DIPayload = 0;
  byte DOPayload = 0;
  String AIPayload = "";
  String AOPayload = "";

  unsigned long prevMillisDisplay;

public:
  MainSystem(DigitalInput *digitalInputs, DigitalOutput *digitalOutputs, AnalogInput *analogInputs, AnalogOutput *analogOutputs)
    : digitalInputs(digitalInputs), digitalOutputs(digitalOutputs), analogInputs(analogInputs), analogOutputs(analogOutputs) {
  }

  void loop() {
    /*=== Listen Input Ports ===*/
    this->listen();

    /*=== For publish, pre-build payloads ===*/
    this->buildPayloads();

    /*=== For publish, prepare pub msg ===*/
    this->preparePubMsg();

    /*=== For subscribe (DO & AO only) ===*/
    this->commandHook();

    /*=== Display (for NBIoT, DO, AO, DI, AI)===*/
    this->handleDisplayContent();
  }

  void listen() {
    for (size_t i = 0; i < DI_NUMS; i++) {
      digitalInputs[i].listen();
    }

    for (size_t i = 0; i < AI_NUMS; i++) {
      analogInputs[i].listen();
      analogInputs[i].getReading();
    }
  }

  void buildPayloads() {
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

    /*=== Debug ===*/
    Serial.print(this->DIPayload);
    Serial.print(" | ");
    Serial.print(this->DOPayload);
    Serial.print(" | ");
    Serial.print(this->AIPayload);
    Serial.print(" | ");
    Serial.print(this->AOPayload);
    Serial.println();
  }

  void preparePubMsg() {
    pubMsgContent = "{\"csq\":";
    pubMsgContent.concat(nbiot.CSQ);
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"cgatt\":");
    pubMsgContent.concat(nbiot.CGATT);
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"cereg\":\"");
    pubMsgContent.concat(nbiot.CEREG);
    pubMsgContent.concat("\"");
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"din\":");
    pubMsgContent.concat(String(this->DIPayload));
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"dout\":");
    pubMsgContent.concat(String(this->DOPayload));
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"ain\":");
    pubMsgContent.concat(this->AIPayload);
    pubMsgContent.concat(",");
    pubMsgContent.concat("\"current\":");
    pubMsgContent.concat(this->AOPayload);
    pubMsgContent.concat("}");

    int contentLen = pubMsgContent.length();

    pubMsgPrepare = "AT+QMTPUB=0,0,0,0,rgt/";
    pubMsgPrepare.concat(nbiot.IMEI);
    pubMsgPrepare.concat("/in,");
    pubMsgPrepare.concat(String(contentLen));

    pubMsgForce = pubMsgPrepare;
    pubMsgForce.concat(",");
    pubMsgForce.concat(pubMsgContent);
  }

  void commandHook() {
    String msg = nbiot.readRecvMsg();

    if (msg.length() <= 0) {
      return;
    }

    Serial.println(msg);
  }

  void handleDisplayContent() {
    if (millis() - this->prevMillisDisplay > 1000) {
      displayClient.prepareBuffer(0, 18 + random(5), random(256), random(256), analogInputs, analogOutputs);
      displayClient.sendBuffer();
      displayClient.debug();
      this->prevMillisDisplay = millis();
    }
  }

  ~MainSystem() {}
};

#endif