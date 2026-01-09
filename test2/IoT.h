#include "Globals.h"
#include "AsyncTimer.h"

#ifndef IOT_H
#define IOT_H

AsyncTimer iotParamTimer(1000);
AsyncTimer iotStateTimer(1000);

class IoT {
private:
  void listen() {
    while (SerialIoT.available() > 0) {
      char c = SerialIoT.read();
      iotSerialRecv += c;
    }
  }

  void consume() {
    int delimiterIndex = -1;

    {  // boundary protection
      delimiterIndex = iotSerialRecv.length();
    }

    {  // delimiter check
      delimiterIndex = iotSerialRecv.indexOf(F("\r"));
      if (delimiterIndex > -1) {
        delimiterIndex = iotSerialRecv.indexOf(F("\n"));
      }
    }

    if (delimiterIndex > -1) {
      {
        iotExtractedRecv = iotSerialRecv.substring(0, delimiterIndex);
        if (iotExtractedRecv == F("\r") || iotExtractedRecv == F("\n") || iotExtractedRecv == F("\r\n") || iotExtractedRecv == F("\n\r")) {
          iotExtractedRecv = F("");
        }
      }
      {
        iotSerialRecv = iotSerialRecv.substring(delimiterIndex + 1);
      }
    } else {
      iotExtractedRecv = F("");
    }
  }

  void queryParams() {
    if (mqttPublishLock.isReleased()) {
      if (iotParamTimer.asyncDelay(5000)) {
        this->printlnFlush(F("AT+CPIN?"));
        this->printlnFlush(F("AT+CSQ"));
        this->printlnFlush(F("AT+CGATT?"));
        this->printlnFlush(F("AT+CEREG?"));
      }
    }
  }

  void monitorParams() {
    // === IMEI ===
    {
      iotCmpStr = F("+CGSN: ");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);

      if (iotCmpStrIdx > -1) {
        {
          iotIMEI = iotExtractedRecv.substring(8, 8 + 15);
        }

        mqttConnCmd = F("AT+QMTCONN=0,dev_");
        mqttConnCmd.concat(iotIMEI);
        mqttConnCmd.concat(F(",tswh,1Wo=[6vA0m"));

        mqttSubsCmd = F("AT+QMTSUB=0,1,rgt/");
        mqttSubsCmd.concat(iotIMEI);
        mqttSubsCmd.concat(F("/out,0"));
      }
    }

    // === CSQ ===
    {
      iotCmpStr = F("+CSQ: ");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        int ws = iotExtractedRecv.indexOf(F(": "));
        int we = iotExtractedRecv.indexOf(F(","));
        {
          iotCSQ = iotExtractedRecv.substring(ws + 2, we);
        }

        byte CSQIdx = 0;
        {
          CSQIdx = iotCSQ.toInt();
        }
        if (CSQIdx == 99 || CSQIdx <= 3) {
          iotCSQErrCnt.accu();
        } else {
          iotCSQErrCnt.reset();
        }
      }
    }

    // === CGATT ===
    {
      iotCmpStr = F("+CGATT: ");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        {
          iotCGATT = iotExtractedRecv.substring(8, 9);
        }
        if (iotCGATT == F("1")) {
          if (iotConnState == IOT_STATE_WAITING_CGATT) {
            iotConnState = IOT_STATE_FINISH_CGATT;
          }
          iotCGATTErrCnt.reset();
        } else {
          iotCGATTErrCnt.accu();
        }
      }
    }

    // === CEREG ===
    {
      iotCmpStr = F("+CEREG: ");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        {
          iotCEREG = iotExtractedRecv.substring(8, 11);
        }

        if (iotCEREG == F("0,1")) {
          if (iotConnState == IOT_STATE_WAITING_CEREG) {
            iotConnState = IOT_STATE_FINISH_CEREG;
          }
          iotCEREGErrCnt.reset();
        } else {
          iotCEREGErrCnt.accu();
        }
      }
    }
  }

  void stateManagement() {
    if (iotConnState == IOT_STATE_WAITING_INIT) {
      Serial.println(">>> INIT, RESET");
      iotConnState = IOT_STATE_WAITING_RESET;
      iotConnState = IOT_STATE_WAITING_RESET_HARDWARE;
    }

    if (iotConnState == IOT_STATE_WAITING_RESET_HARDWARE) {
      digitalWrite(IOT_MODULE_RESET_PIN, LOW);
      if (iotStateTimer.asyncDelay(1000)) {
        digitalWrite(IOT_MODULE_RESET_PIN, HIGH);
        iotConnState = IOT_STATE_FINISH_RESET_HARDWARE;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_RESET_HARDWARE) {
      if (iotStateTimer.asyncDelay(1000)) {
        this->printlnFlush(F("AT+CFUN=1,1"));
        iotConnState = IOT_STATE_WAITING_RESET_SOFTWARE;
      }
    }

    if (iotConnState == IOT_STATE_WAITING_RESET_SOFTWARE) {
      iotCmpStr = F("RDY");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        Serial.println(F("\r\n>>> IOT READY"));
        iotConnState = IOT_STATE_FINISH_RESET_SOFTWARE;
        iotConnState = IOT_STATE_FINISH_RESET;
        this->printlnFlush(F("ATI"));
      }
    }

    if (iotConnState == IOT_STATE_FINISH_RESET) {
      iotConnState = IOT_STATE_WAITING_CONFIG;
    }

    if (iotConnState == IOT_STATE_WAITING_CONFIG) {
      if (iotStateTimer.asyncDelay(500)) {
        this->printlnFlush(F("ATE0"));
        this->printlnFlush(F("AT+CGSN=1"));
        this->printlnFlush(F("AT+QSCLK=0"));
        this->printlnFlush(F("AT+QIDNSCFG=0,223.5.5.5,8.8.8.8"));
        this->printlnFlush(F("AT+CFUN=1"));
        this->printlnFlush(F("AT+QSCLK=0"));
        // this->printlnFlush(F("AT+CPSMS=0")); // for nbiot
        // this->printlnFlush(F("AT+CSCON=0")); // for nbiot
        // this->printlnFlush(F("AT+CEDRXS=0,5")); // for nbiot
        this->printlnFlush(F("AT+QMTCLOSE=0"));
        this->printlnFlush(F("AT+QMTDISC=0"));

        iotConnState = IOT_STATE_FINISH_CONFIG;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CONFIG) {
      if (iotStateTimer.asyncDelay(500)) {
        this->printlnFlush(F("AT+CSQ"));
        iotConnState = IOT_STATE_WAITING_CSQ;
      }
    }

    if (iotConnState == IOT_STATE_WAITING_CSQ) {
      iotConnState = IOT_STATE_FINISH_CSQ;
    }

    if (iotConnState == IOT_STATE_FINISH_CSQ) {
      iotConnState = IOT_STATE_WAITING_CGATT;
    }

    if (iotConnState == IOT_STATE_WAITING_CGATT) {
      if (iotStateTimer.asyncDelay(1000)) {
        this->printlnFlush(F("AT+CGATT?"));
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CGATT) {
      iotConnState = IOT_STATE_WAITING_CEREG;
    }

    if (iotConnState == IOT_STATE_WAITING_CEREG) {
      if (iotStateTimer.asyncDelay(1000)) {
        this->printlnFlush(F("AT+CEREG?"));
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CEREG) {
      iotConnState = IOT_STATE_WAITING_OPEN_MQTT;
    }

    if (iotConnState == IOT_STATE_WAITING_OPEN_MQTT) {
      if (iotStateTimer.asyncDelay(1000)) {
        this->printlnFlush(F("AT+QMTOPEN=0,iot.rec-gt.com,1880"));
      }

      iotCmpStr = F("+QMTOPEN: 0,0");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        iotConnState = IOT_STATE_FINISH_OPEN_MQTT;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_OPEN_MQTT) {
      iotConnState = IOT_STATE_WAITING_CONN_MQTT;
    }

    if (iotConnState == IOT_STATE_WAITING_CONN_MQTT) {
      if (iotStateTimer.asyncDelay(1000)) {
        this->printlnFlush(mqttConnCmd);
      }

      iotCmpStr = F("+QMTCONN: 0,0,0");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        iotConnState = IOT_STATE_FINISH_CONN_MQTT;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CONN_MQTT) {
      iotConnState = IOT_STATE_WAITING_SUBS_MQTT_TOPIC;
    }

    if (iotConnState == IOT_STATE_WAITING_SUBS_MQTT_TOPIC) {
      if (iotStateTimer.asyncDelay(1000)) {
        this->printlnFlush(mqttSubsCmd);
      }

      iotCmpStr = F("+QMTSUB: 0,1,0,0");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        iotConnState = IOT_STATE_FINISH_SUBS_MQTT_TOPIC;
        iotConnState = IOT_STATE_FINISH_INIT;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_INIT) {
      iotConnState = IOT_PIPELINE_INIT;
    }

    if (iotConnState == IOT_PIPELINE_INIT) {
      if (forcePublishMode) {
        mqttPublishLock.lock();
        this->printlnFlush(mqttPublMsgPrepare);
        Serial.println(mqttPublMsgPrepare);
        iotConnState = IOT_PIPELINE_WAITING_PREPARE_PUBMSG;
        forcePublishMode = false;
      } else {
        if (iotStateTimer.asyncDelay(30000UL)) {
          mqttPublishLock.lock();
          this->printlnFlush(mqttPublMsgPrepare);
          Serial.println(mqttPublMsgPrepare);
          iotConnState = IOT_PIPELINE_WAITING_PREPARE_PUBMSG;
        }
      }
    }

    if (iotConnState == IOT_PIPELINE_WAITING_PREPARE_PUBMSG) {
      iotCmpStr = F("> ");
      iotCmpStrIdx = iotSerialRecv.indexOf(iotCmpStr);  // === special case for ">" ===
      if (iotCmpStrIdx > -1) {
        iotConnState = IOT_PIPELINE_FINISH_PREPARE_PUBMSG;
        iotPublishErrCnt.reset();
      } else {
        if (iotStateTimer.asyncDelay(1000)) {
          iotPublishErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_PIPELINE_FINISH_PREPARE_PUBMSG) {
      Serial.println(mqttPublMsgPayload);
      this->printlnFlush(mqttPublMsgPayload);
      mqttPublishLock.release();
      iotConnState = IOT_PIPELINE_WAITING_PUBLISH;
      iotConnState = IOT_STATE_FINISH_INIT;
    }
  }

  void errHook() {
    if (
      iotExtractedRecv.indexOf(F("+QIURC: \"pdpdeact\",1")) > -1
      || iotExtractedRecv.indexOf(F("+CME ERROR")) > -1
      || iotPublishErrCnt.over(3)
      || iotCSQErrCnt.over(10)
      || iotCGATTErrCnt.over(10)
      || iotCEREGErrCnt.over(10)) {
      iotConnState = IOT_STATE_WAITING_INIT;
    }
  }

  void mqttMsgBuilder() {
    // 1. build payload
    mqttPublMsgPayload = F("{\"csq\":");
    mqttPublMsgPayload.concat(iotCSQ);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"din\":"));
    mqttPublMsgPayload.concat(255);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"dout\":"));
    mqttPublMsgPayload.concat(255);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"ain\":"));
    mqttPublMsgPayload.concat(F("["));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 4096));
    mqttPublMsgPayload.concat(F("]"));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"aout\":"));
    mqttPublMsgPayload.concat(F("["));
    mqttPublMsgPayload.concat(random(0, 255));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 255));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 255));
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(random(0, 255));
    mqttPublMsgPayload.concat(F("]"));
    mqttPublMsgPayload.concat(F("}"));

    // 2. build prepare msg
    mqttPublMsgPrepare = F("AT+QMTPUBEX=0,0,0,0,rgt/");
    mqttPublMsgPrepare.concat(iotIMEI);
    mqttPublMsgPrepare.concat(F("/in,"));
    mqttPublMsgPrepare.concat(mqttPublMsgPayload.length() - (random() % 2 == 0 ? 0 : 1));
  }

public:
  void init() {
    SerialIoT.begin(115200);
    pinMode(IOT_MODULE_RESET_PIN, OUTPUT);
    digitalWrite(IOT_MODULE_RESET_PIN, HIGH);
    iotConnState = IOT_STATE_WAITING_INIT;
  }

  void loop() {
    this->listen();
    this->consume();
    this->stateManagement();
    this->queryParams();
    this->monitorParams();
    this->errHook();
    this->printExtractedRecv();
  }

  void buildMsg() {
    this->mqttMsgBuilder();
  }

  void printlnFlush(const String& cmd) {
    SerialIoT.println(cmd);
    SerialIoT.flush();
    delay(1);
  }

  void forcePublish() {
    if (IOT_PIPELINE_INIT <= iotConnState && iotConnState <= IOT_PIPELINE_FINISH_PUBLISH) {
      forcePublishMode = true;
    }
  }

  void printExtractedRecv() {
    if (iotExtractedRecv != F("")) {
      Serial.print("[[");
      Serial.print(iotExtractedRecv);
      Serial.println("]]");
    }
  }

  void printSerialRecv() {
    Serial.print("[[");
    Serial.print(iotSerialRecv);
    Serial.println("]]");
  }

  void printParameterState() {
    if (iotStateTimer.asyncDelay(1000)) {
      Serial.println("iotCSQ: " + iotCSQ);
      Serial.println("iotIMEI: " + iotIMEI);
      Serial.println("iotCGATT: " + iotCGATT);
      Serial.println("iotCEREG: " + iotCEREG);
    }
  }
};

extern IoT iot;

#endif

// A  T  +  C  S  Q  +  C  S  Q  :  _  2  4  ,  9  9  O  K
// 41 54 2B 43 53 51 2B 43 53 51 3A 20 32 34 2C 39 39 4F 4B 0
