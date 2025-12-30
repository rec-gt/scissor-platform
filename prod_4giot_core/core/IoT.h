#include "./AsyncTimer.h"
#include "./Utils.h"
#include "./Watchdog.h"
#include "./Globals.h"

#ifndef IoTService_h
#define IoTService_h

#define IoTServiceSerial Serial1

Watchdog iotWatchdog(30000UL);

AsyncTimer iotTimer(10000UL);

bool iotSoftReset = false;

class IoTService {
private:
  enum IOT_STATE {
    IOT_STATE_WAITING_INIT,
    
    IOT_STATE_WAITING_RESET,
    IOT_STATE_FINISH_RESET,
    IOT_STATE_WAITING_RDY,
    IOT_STATE_FINISH_RDY,
    IOT_STATE_WAITING_IP,
    IOT_STATE_FINISH_IP,
    IOT_STATE_WAITING_CONFIG,
    IOT_STATE_FINISH_CONFIG,
    IOT_STATE_WAITING_CSQ,
    IOT_STATE_FINISH_CSQ,
    IOT_STATE_WAITING_CGATT,
    IOT_STATE_FINISH_CGATT,
    IOT_STATE_WAITING_CEREG,
    IOT_STATE_FINISH_CEREG,
    IOT_STATE_WAITING_OPEN,
    IOT_STATE_FINISH_OPEN,
    IOT_STATE_WAITING_CONN,
    IOT_STATE_FINISH_CONN,
    IOT_STATE_WAITING_SUB,
    IOT_STATE_FINISH_SUB,

    IOT_STATE_FINISH_INIT,
  };

  enum PUBSUB_PIPELINE {
    PIPELINE_DEFAULT,
    PIPELINE_WAITING_CSQ,
    PIPELINE_FINISH_CSQ,
    PIPELINE_WAITING_CGATT,
    PIPELINE_FINISH_CGATT,
    PIPELINE_WAITING_CEREG,
    PIPELINE_FINISH_CEREG,
    PIPELINE_WAITING_PREPARE_PUBMSG,
    PIPELINE_FINISH_PREPARE_PUBMSG,
    PIPELINE_WAITING_PUBLISH,
    PIPELINE_FINISH_PUBLISH,
  };

  bool finishInit = false;

  byte resetPin = 24;  // to be assign

  bool debugMode = false;

  void clearSerialBuffer() {
    while (IoTServiceSerial.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    iotSerialRecv = F("");
  }

  void printlnFlush(const String& cmd, unsigned int delayTime = 2) {
    IoTServiceSerial.println(cmd);
    IoTServiceSerial.flush();
    delay(delayTime);
  }

public:
  IOT_STATE connState = STATE_WAITING_RESET;
  PUBSUB_PIPELINE pipelineState = PIPELINE_DEFAULT;

  // important, do not remove
  bool pubMsgPayloadLock = false;

  IoTService() {
    IoTServiceSerial.begin(115200);
    pinMode(this->resetPin, OUTPUT);
    digitalWrite(this->resetPin, HIGH);
  }

  void resetBuffers() {
    this->clearSerialBuffer();
    this->clearResBuffer();
    delay(10);
  }

  void debug() {
    this->debugMode = true;
  }

  void init(bool asyncInitMode = false) {
    Serial.println(F("\r\n=== IOT START ==="));

    iotWatchdog.enable();
    iotWatchdog.setCallback([]() {
      iotSoftReset = true;
    });

    this->resetBuffers();

    if (asyncInitMode) {
      this->finishInit = true;
    } else {
      this->finishInit = false;
      this->loop();
    }
  }

  void loop() {
    while (1) {
      iotWatchdog.monitor();

      this->ask();
      this->listen();

      if (this->finishInit) {
        break;
      } else {
        delay(10);
      }
    }

    // update global variable
    iotConnState = this->connState;
  }

  void ask() {
    if (this->connState == STATE_WAITING_RESET) {
      iotSerialRecv = F("");
      iotConnCmd = F("");
      iotSubsCmd = F("");

      iotCSQ = F("");
      iotIMEI = F("");
      iotCGATT = F("");
      iotCEREG = F("");

      iotPubMsgPayload = F("");
      iotPubMsgPrepare = F("");
      iotPubMsgCommand = F("");

      this->pubMsgPayloadLock = false;

      digitalWrite(this->resetPin, LOW);
      if (iotTimer.autoExpired(1000)) {
        digitalWrite(this->resetPin, HIGH);
        this->connState = STATE_FINISH_RESET;
        Serial.println(F("\r\nWaiting IP"));
      }
    }

    if (this->connState == STATE_FINISH_RESET) {
      if (iotTimer.autoExpired(500)) {
        this->printlnFlush(F("AT+CGSN=1"));
        this->printlnFlush(F("AT+QSCLK=0"));
        // this->printlnFlush(F("AT+QIDNSCFG=0,223.5.5.5,8.8.8.8"));
        this->connState = STATE_WAITING_IP;
      }
    }

    if (this->connState == STATE_FINISH_IP) {
      Serial.println(F("\r\nSETTING UP IOT"));
      this->printlnFlush(F("AT+CFUN=1"));
      this->printlnFlush(F("AT+QSCLK=0"));
      // this->printlnFlush(F("AT+CPSMS=0"));
      // this->printlnFlush(F("AT+CSCON=0"));
      // this->printlnFlush(F("AT+CEDRXS=0,5"));
      this->printlnFlush(F("AT+QMTCLOSE=0"));
      this->printlnFlush(F("AT+QMTDISC=0"));
      this->connState = STATE_WAITING_SETUP;
    }

    if (this->connState == STATE_FINISH_SETUP) {
      if (iotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nGETTING CSQ"));

        this->printlnFlush(F("AT+CSQ"));

        this->connState = STATE_WAITING_CSQ;
      }
    }

    if (this->connState == STATE_FINISH_CSQ) {
      if (iotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nGETTING CGATT"));

        this->printlnFlush(F("AT+CGATT?"));

        this->connState = STATE_WAITING_CGATT;
      }
    }

    if (this->connState == STATE_FINISH_CGATT) {
      if (iotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nGETTING CEREG"));

        this->printlnFlush(F("AT+CEREG?"));

        this->connState = STATE_WAITING_CEREG;
      }
    }

    if (this->connState == STATE_FINISH_CEREG) {
      if (iotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nOPENING MQTT..."));
        // this->printlnFlush(F("AT+QMTOPEN=0,8.210.84.24,1880"));
        this->printlnFlush(F("AT+QMTOPEN=0,iot.rec-gt.com,1880"));

        this->connState = STATE_WAITING_OPEN;
      }
    }

    if (this->connState == STATE_FINISH_OPEN) {
      if (iotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nCONNECTING MQTT..."));

        this->printlnFlush(iotConnCmd);
        this->connState = STATE_WAITING_CONN;
      }
    }

    if (this->connState == STATE_FINISH_CONN) {
      Serial.println(F("\r\nSUBSCRBING TOPIC..."));

      this->printlnFlush(iotSubsCmd);
      this->connState = STATE_WAITING_SUB;
    }

    if (this->connState == STATE_FINISH_SUB) {
      this->finishInit = true;
      this->connState = STATE_FINISH_IOT_INIT;
      Serial.println(F("\r\nFINISH INIT IOT"));
    }


    if (this->connState == STATE_FINISH_IOT_INIT) {
      if (this->pipelineState == PIPELINE_DEFAULT) {
        if (iotTimer.autoExpired(5000)) {
          Serial.println(F("\r\n[CONNECTED] QUERYING CSQ"));
          this->printlnFlush(F("AT+CSQ"));

          this->pipelineState = PIPELINE_WAITING_CSQ;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CSQ) {
        if (iotTimer.autoExpired(5000)) {
          Serial.println(F("\r\n[CONNECTED] QUERYING CGATT"));
          this->printlnFlush(F("AT+CGATT?"));

          this->pipelineState = PIPELINE_WAITING_CGATT;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CGATT) {
        if (iotTimer.autoExpired(5000)) {
          Serial.println(F("\r\n[CONNECTED] QUERYING CEREG"));
          this->printlnFlush(F("AT+CEREG?"));

          this->pipelineState = PIPELINE_WAITING_CEREG;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CEREG) {
        if (iotTimer.autoExpired(13000)) {
          Serial.println(F("\r\nEXECUTE REGULAR PUBLISH"));

          this->printlnFlush(iotPubMsgPrepare);
          this->pubMsgPayloadLock = true;  // disable the preparation of payload
          this->pipelineState = PIPELINE_WAITING_PREPARE_PUBMSG;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_PREPARE_PUBMSG) {
        if (iotTimer.autoExpired(2000)) {
          this->printlnFlush(iotPubMsgPayload);
          this->pubMsgPayloadLock = false;  // release the lock
          this->pipelineState = PIPELINE_WAITING_PUBLISH;
        }
      }
    }
  }

  void listen() {
    if (IoTServiceSerial.available() > 0) {
      while (IoTServiceSerial.available() > 0) {
        char c = IoTServiceSerial.read();

        if (this->debugMode) {
          Serial.print(c);
        }

        if (c != '\r' && c != '\n') {
          iotSerialRecv += c;
        }

        if (c == '\r') {
          this->answer();
          this->handleReadMsg();
          this->clearResBuffer();
        }
      }
    }

    if (iotSoftReset) {
      this->resetBuffers();
      iotSoftReset = false;
      this->connState = STATE_WAITING_RESET;
      this->pipelineState = PIPELINE_DEFAULT;
      Serial.println(F("[SOFT_RESET]"));
    }
  }

  void answer() {
    int idx = -1;

    if (this->connState == STATE_WAITING_IP) {
      cmpStr = F("+IP:");
      idx = iotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nFINISH WAITING IP"));

        this->connState = STATE_FINISH_IP;
        iotWatchdog.pet();
      } else {
        if (iotTimer.autoExpired(1000)) {
          this->connState = STATE_FINISH_IP;  // by- pass
        }
      }
    }

    if (this->connState == STATE_WAITING_SETUP) {
      {
        Serial.println(F("\r\nFINISH SETUP"));

        this->connState = STATE_FINISH_SETUP;
        iotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CSQ) {
      cmpStr = F("+CSQ:");
      idx = iotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nFINISH GETTING CSQ"));

        this->connState = STATE_FINISH_CSQ;
        iotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CGATT) {
      cmpStr = F("+CGATT:");
      idx = iotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nFINISH GETTING CGATT"));

        this->connState = STATE_FINISH_CGATT;
        iotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CEREG) {
      cmpStr = F("+CEREG:");
      idx = iotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nFINISH GETTING CEREG"));

        this->connState = STATE_FINISH_CEREG;
        iotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_OPEN) {
      cmpStr = F("+QMTOPEN: 0,0");
      idx = iotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nOPENED MQTT"));

        this->connState = STATE_FINISH_OPEN;
        iotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CONN) {
      cmpStr = F("+QMTCONN: 0,0,0");
      idx = iotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nCONNECTED MQTT"));

        this->connState = STATE_FINISH_CONN;
        iotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_SUB) {
      cmpStr = F("+QMTSUB:");
      idx = iotSerialRecv.indexOf(cmpStr);

      if (idx > -1) {
        Serial.println(F("\r\nSUB TOPIC OK"));

        this->connState = STATE_FINISH_SUB;
        iotWatchdog.pet();
      }
    }

    if (this->connState == STATE_FINISH_IOT_INIT) {
      int idx = -1;
      if (this->pipelineState == PIPELINE_WAITING_CSQ) {

        cmpStr = F("+CSQ:");
        idx = iotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          Serial.println(F("\r\nFINISH GETTING CSQ"));

          this->pipelineState = PIPELINE_FINISH_CSQ;
          iotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CGATT) {

        cmpStr = F("+CGATT:");
        idx = iotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          Serial.println(F("\r\nFINISH GETTING CGATT"));

          this->pipelineState = PIPELINE_FINISH_CGATT;
          iotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CEREG) {

        cmpStr = F("+CEREG:");
        idx = iotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          Serial.println(F("\r\nFINISH GETTING CEREG"));

          this->pipelineState = PIPELINE_FINISH_CEREG;
          iotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PREPARE_PUBMSG) {

        cmpStr = F(">");
        idx = iotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          this->pipelineState = PIPELINE_FINISH_PREPARE_PUBMSG;
          iotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PUBLISH) {

        cmpStr = F("+QMTPUB: 0,0,0");
        idx = iotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          this->pipelineState = PIPELINE_DEFAULT;
          Serial.println(F("\r\nFINISH REGULAR PUBLISH"));

          iotWatchdog.pet();
        }
      }
    }
  }

  void handleReadMsg() {
    int idx = -1;

    // === handle IMEI ===

    cmpStr = F("+CGSN:");
    idx = iotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      {
        iotIMEI = iotSerialRecv.substring(7, 7 + 15);

        if (!utils.isNumeric(iotIMEI)) {
          iotSoftReset = true;
        }

        if (iotIMEI.length() != 15) {
          iotSoftReset = true;
        }

        iotConnCmd = F("AT+QMTCONN=0,dev_");
        iotConnCmd.concat(iotIMEI);
        iotConnCmd.concat(F(",tswh,1Wo=[6vA0m"));

        iotSubsCmd = F("AT+QMTSUB=0,1,rgt/");
        iotSubsCmd.concat(iotIMEI);
        iotSubsCmd.concat(F("/out,0"));
      }
    }

    // === handle CGATT ===
    cmpStr = F("+CGATT:");
    idx = iotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      {
        iotCGATT = iotSerialRecv.substring(8, 8 + 1);
      }

      if (iotCGATT != F("1")) {
        iotSoftReset = true;
      }
    }

    // === handle CEREG ===

    cmpStr = F("+CEREG:");
    idx = iotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      {
        iotCEREG = iotSerialRecv.substring(8, 8 + 3);
      }

      if (iotCEREG != F("0,1")) {
        iotSoftReset = true;
      }
    }

    // === handle CSQ ===

    cmpStr = F("+CSQ:");
    idx = iotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      cmpStr = F(": ");
      int winStart = iotSerialRecv.indexOf(cmpStr);
      cmpStr = F(",");
      int winEnd = iotSerialRecv.indexOf(cmpStr);

      {
        iotCSQ = iotSerialRecv.substring(winStart + 2, winEnd);
      }

      if (iotCSQ == F("99")) {
        iotSoftReset = true;
      }

      if (!utils.isNumeric(iotCSQ)) {
        iotSoftReset = true;
        iotCSQ = F("0");
      }

      {
        int numCSQ = iotCSQ.toInt();
        if (!(numCSQ >= 5 && numCSQ <= 31)) {
          iotSoftReset = true;
          iotCSQ = F("0");
        }
      }
    }

    // === handle publish ACK ===
    cmpStr = F("+QMTPUB:");
    idx = iotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {

      {
        iotPubAck = iotSerialRecv.substring(9, 9 + 5);
      }

      if (iotPubAck != F("0,0,0")) {
        iotSoftReset = true;
      }
    }

    // === handle SUB ACK ===
    cmpStr = F("+QMTSUB:");
    idx = iotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {

      {
        iotSubAck = iotSerialRecv.substring(9, 9 + 7);
      }

      if (iotSubAck != F("0,1,0,0")) {
        iotSoftReset = true;
      }
    }

    // === handle SUB received msg and parse it's content ===
    cmpStr = F("+QMTRECV:");
    idx = iotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      {
        iotSubMsgContent = iotSerialRecv.substring(41, 46);
        Serial.println(iotSubMsgContent);
      }
    }
  }

  void forcePublish() {
    if (this->connState == STATE_FINISH_IOT_INIT) {
      if (this->pipelineState == PIPELINE_WAITING_PREPARE_PUBMSG
          || this->pipelineState == PIPELINE_FINISH_PREPARE_PUBMSG
          || this->pipelineState == PIPELINE_WAITING_PUBLISH
          || this->pipelineState == PIPELINE_FINISH_PUBLISH) {
        return;
      } else {
        Serial.println(F("\r\nFORCE PUBLISH STOPPED"));

        this->printlnFlush(iotPubMsgCommand);
      }
    }
  }

  ~IoTService() {}
};

extern IoTService iotService;

#endif