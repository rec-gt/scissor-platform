#include "Globals.h"
#include "Timer.h"

#ifndef IOT_H
#define IOT_H

Timer iotParamTimer;
Timer iotStateTimer;
Timer mqttStateTimer;

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
      if (iotParamTimer.autoTimeout(5000)) {
        Serial.println(F("Query?"));
        this->printlnFlush(F("AT+CPIN?"));
        this->printlnFlush(F("AT+CSQ"));
        this->printlnFlush(F("AT+CGATT?"));
        this->printlnFlush(F("AT+CEREG?"));
      }
    }
  }

  void monitorParams() {
    /* === IMEI === */
    {
      iotCmpStrIdx = iotExtractedRecv.indexOf(F("+CGSN: "));

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

    /* === CSQ === */
    {
      iotCmpStrIdx = iotExtractedRecv.indexOf(F("+CSQ: "));
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

    /* === CGATT === */
    {
      iotCmpStrIdx = iotExtractedRecv.indexOf(F("+CGATT: "));
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

    /* === CEREG === */
    {
      iotCmpStrIdx = iotExtractedRecv.indexOf(F("+CEREG: "));
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
      Serial.println(F(">>> INIT, RESET"));

      iotCSQErrCnt.reset();
      iotCGATTErrCnt.reset();
      iotCEREGErrCnt.reset();
      mqttOpenErrCnt.reset();
      mqttConnErrCnt.reset();
      mqttPublErrCnt.reset();

      mqttPublishLock.release();
      forcePublishMode = false;

      iotConnState = IOT_STATE_WAITING_RESET;
      iotConnState = IOT_STATE_WAITING_RESET_HARDWARE;
    }

    if (iotConnState == IOT_STATE_WAITING_RESET_HARDWARE) {
      digitalWrite(IOT_MODULE_RESET_PIN, LOW);
      if (iotStateTimer.autoTimeout(1000)) {
        digitalWrite(IOT_MODULE_RESET_PIN, HIGH);
        iotConnState = IOT_STATE_FINISH_RESET_HARDWARE;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_RESET_HARDWARE) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CFUN=1,1"));
        iotConnState = IOT_STATE_WAITING_RESET_SOFTWARE;
      }
    }

    if (iotConnState == IOT_STATE_WAITING_RESET_SOFTWARE) {
      iotCmpStrIdx = iotExtractedRecv.indexOf(F("RDY"));
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
      if (iotStateTimer.autoTimeout(500)) {
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
      if (iotStateTimer.autoTimeout(500)) {
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
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CGATT?"));
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CGATT) {
      iotConnState = IOT_STATE_WAITING_CEREG;
    }

    if (iotConnState == IOT_STATE_WAITING_CEREG) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+CEREG?"));
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CEREG) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(F("AT+QMTOPEN=0,iot.rec-gt.com,1880"));
        iotConnState = IOT_STATE_WAITING_OPEN_MQTT;
      }
    }

    if (iotConnState == IOT_STATE_WAITING_OPEN_MQTT) {
      if (iotExtractedRecv.indexOf(F("+QMTOPEN: 0,0")) > -1) {
        iotConnState = IOT_STATE_FINISH_OPEN_MQTT;
        mqttOpenErrCnt.reset();
      } else {
        if (mqttStateTimer.autoTimeout(1000)) {
          mqttOpenErrCnt.accu();
        }
      }
    }

    // TODO: add ErrCnt to catch err and reboot
    if (iotConnState == IOT_STATE_FINISH_OPEN_MQTT) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(mqttConnCmd);
        iotConnState = IOT_STATE_WAITING_CONN_MQTT;
      }
    }

    if (iotConnState == IOT_STATE_WAITING_CONN_MQTT) {
      if (iotExtractedRecv.indexOf(F("+QMTCONN: 0,0,0")) > -1) {
        iotConnState = IOT_STATE_FINISH_CONN_MQTT;
        mqttConnErrCnt.reset();
      } else {
        if (mqttStateTimer.autoTimeout(1000)) {
          mqttConnErrCnt.accu();
        }
      }
    }

    if (iotConnState == IOT_STATE_FINISH_CONN_MQTT) {
      if (iotStateTimer.autoTimeout(1000)) {
        this->printlnFlush(mqttSubsCmd);
        iotConnState = IOT_STATE_WAITING_SUBS_MQTT_TOPIC;
      }
    }

    if (iotConnState == IOT_STATE_WAITING_SUBS_MQTT_TOPIC) {
      iotCmpStrIdx = iotExtractedRecv.indexOf(F("+QMTSUB: 0,1,0,0"));
      if (iotCmpStrIdx > -1) {
        iotConnState = IOT_STATE_FINISH_SUBS_MQTT_TOPIC;
        iotConnState = IOT_STATE_FINISH_INIT;
        mqttSubsErrCnt.reset();
      } else {
        if (mqttStateTimer.autoTimeout(1000)) {
          mqttSubsErrCnt.accu();
        }
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
        if (iotStateTimer.autoTimeout(30000)) {
          mqttPublishLock.lock();
          this->printlnFlush(mqttPublMsgPrepare);
          Serial.println(mqttPublMsgPrepare);
          iotConnState = IOT_PIPELINE_WAITING_PREPARE_PUBMSG;
        }
      }
    }

    if (iotConnState == IOT_PIPELINE_WAITING_PREPARE_PUBMSG) {
      iotCmpStrIdx = iotSerialRecv.indexOf(F("> "));  // === special case for ">" ===
      if (iotCmpStrIdx > -1) {
        iotConnState = IOT_PIPELINE_FINISH_PREPARE_PUBMSG;
        mqttPublErrCnt.reset();
      } else {
        if (iotStateTimer.autoTimeout(1000)) {
          mqttPublErrCnt.accu();
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

  void handleSubs() {
    {
      iotCmpStrIdx = iotExtractedRecv.indexOf(F("+QMTRECV: "));
      if (iotCmpStrIdx > -1) {
        {
          mqttSubsMsgContent = iotExtractedRecv.substring(41, 46);
          Serial.println(mqttSubsMsgContent);
        }
      }
    }
  }

  void errHook() {
    if (iotExtractedRecv.indexOf(F("+QIURC: \"pdpdeact\",1")) > -1) {
      iotConnState = IOT_STATE_WAITING_INIT;
    }

    if (iotExtractedRecv.indexOf(F("+CME ERROR")) > -1) {
      iotConnState = IOT_STATE_WAITING_INIT;
    }

    if (iotCSQErrCnt.over(10)) {
      Serial.print(F("iotCSQErrCnt.over(10)"));
      iotConnState = IOT_STATE_WAITING_INIT;
    }

    if (iotCGATTErrCnt.over(10)) {
      Serial.print(F("iotCGATTErrCnt.over(10)"));
      iotConnState = IOT_STATE_WAITING_INIT;
    }

    if (iotCEREGErrCnt.over(10)) {
      Serial.print(F("iotCEREGErrCnt.over(10)"));
      iotConnState = IOT_STATE_WAITING_INIT;
    }

    if (mqttOpenErrCnt.over(5)) {
      Serial.print(F("mqttOpenErrCnt.over(5)"));
      iotConnState = IOT_STATE_WAITING_INIT;
    }

    if (mqttConnErrCnt.over(5)) {
      Serial.print(F("mqttConnErrCnt.over(5)"));
      iotConnState = IOT_STATE_WAITING_INIT;
    }

    if (mqttPublErrCnt.over(5)) {
      Serial.print(F("mqttPublErrCnt.over(5)"));
      iotConnState = IOT_STATE_WAITING_INIT;
    }
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
    this->handleSubs();
    this->errHook();
    this->printExtractedRecv();
  }

  void printlnFlush(const String& cmd) {
    SerialIoT.println(cmd);
    SerialIoT.flush();
    delay(1);
  }

  void buildMsg(byte _DIPayload, byte _DOPayload, const String& _AIPayload, const String& _AOPayload) {
    // 1. build payload
    mqttPublMsgPayload = F("{\"csq\":");
    mqttPublMsgPayload.concat(iotCSQ);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"din\":"));
    mqttPublMsgPayload.concat(_DIPayload);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"dout\":"));
    mqttPublMsgPayload.concat(_DOPayload);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"ain\":"));
    mqttPublMsgPayload.concat(_AIPayload);
    mqttPublMsgPayload.concat(F(","));
    mqttPublMsgPayload.concat(F("\"aout\":"));
    mqttPublMsgPayload.concat(_AOPayload);
    mqttPublMsgPayload.concat(F("}"));

    // 2. build prepare msg
    mqttPublMsgPrepare = F("AT+QMTPUBEX=0,0,0,0,rgt/");
    mqttPublMsgPrepare.concat(iotIMEI);
    mqttPublMsgPrepare.concat(F("/in,"));
    mqttPublMsgPrepare.concat(mqttPublMsgPayload.length());
  }

  void forcePublish() {
    if (IOT_PIPELINE_INIT <= iotConnState && iotConnState <= IOT_PIPELINE_FINISH_PUBLISH) {
      forcePublishMode = true;
    }
  }

  void printExtractedRecv() {
    if (iotExtractedRecv != F("")) {
      Serial.print(F("[["));
      Serial.print(iotExtractedRecv);
      Serial.println(F("]]"));
    }
  }

  void printSerialRecv() {
    Serial.print(F("[["));
    Serial.print(iotSerialRecv);
    Serial.println(F("]]"));
  }
};

extern IoT iot;

#endif

// A  T  +  C  S  Q  +  C  S  Q  :  _  2  4  ,  9  9  O  K
// 41 54 2B 43 53 51 2B 43 53 51 3A 20 32 34 2C 39 39 4F 4B 0