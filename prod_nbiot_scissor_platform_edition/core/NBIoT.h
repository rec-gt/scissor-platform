#include "./AsyncTimer.h"
#include "./Utils.h"
#include "./Watchdog.h"
#include "./Globals.h"

#ifndef NBIoT_h
#define NBIoT_h

#define NBIoTSerial Serial1

Watchdog nbiotWatchdog(30000UL);

AsyncTimer nbiotTimer(10000UL);

bool nbiotSoftReset = false;

class NBIoT {
private:
  enum NBIOT_STATE {
    STATE_DEFAULT,
    STATE_WAITING_RESET,
    STATE_FINISH_RESET,
    STATE_WAITING_IP,
    STATE_FINISH_IP,
    STATE_WAITING_SETUP,
    STATE_FINISH_SETUP,
    STATE_WAITING_IMEI,
    STATE_FINISH_IMEI,
    STATE_WAITING_CSQ,
    STATE_FINISH_CSQ,
    STATE_WAITING_CGATT,
    STATE_FINISH_CGATT,
    STATE_WAITING_CEREG,
    STATE_FINISH_CEREG,
    STATE_WAITING_OPEN,
    STATE_FINISH_OPEN,
    STATE_WAITING_CONN,
    STATE_FINISH_CONN,
    STATE_WAITING_SUB,
    STATE_FINISH_SUB,
    STATE_FINISH_NBIOT_INIT,
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
    while (NBIoTSerial.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    nbiotSerialRecv = "";
  }

  void printlnFlush(const String& cmd, unsigned int delayTime = 2) {
    NBIoTSerial.println(cmd);
    NBIoTSerial.flush();
    delay(delayTime);
  }

public:
  NBIOT_STATE connState = STATE_WAITING_RESET;
  PUBSUB_PIPELINE pipelineState = PIPELINE_DEFAULT;

  // important, do not remove
  bool pubMsgPayloadLock = false;

  NBIoT() {
    NBIoTSerial.begin(9600);
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
    Serial.println(F("\r\n=== NBIOT START ==="));

    nbiotWatchdog.enable();
    nbiotWatchdog.setCallback([]() {
      nbiotSoftReset = true;
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
      nbiotWatchdog.monitor();

      this->ask();
      this->listen();

      if (this->finishInit) {
        break;
      } else {
        delay(10);
      }
    }

    // update global variable
    nbiotConnState = this->connState;
  }

  void ask() {
    if (this->connState == STATE_WAITING_RESET) {
      nbiotSerialRecv = "";
      nbIotConnCmd = "";
      nbiotSubsCmd = "";

      nbiotCSQ = "";
      nbiotIMEI = "";
      nbiotCGATT = "";
      nbiotCEREG = "";

      nbiotPubMsgPayload = "";
      nbiotPubMsgPrepare = "";
      nbiotPubMsgCommand = "";

      this->pubMsgPayloadLock = false;

      digitalWrite(this->resetPin, LOW);
      if (nbiotTimer.autoExpired(1000)) {
        digitalWrite(this->resetPin, HIGH);
        this->connState = STATE_FINISH_RESET;
        delay(100);
      }
    }

    if (this->connState == STATE_FINISH_RESET) {
      if (nbiotTimer.autoExpired(1000)) {
        Serial.println(F("\r\nWAITING IP"));

        this->printlnFlush(F("AT+QSCLK=0"));

        this->connState = STATE_WAITING_IP;
      }
    }

    if (this->connState == STATE_FINISH_IP) {
      Serial.println(F("\r\nSETTING UP NBIOT"));

      this->printlnFlush(F("AT+CFUN=1"));

      this->printlnFlush(F("AT+QSCLK=0"));

      this->printlnFlush(F("AT+CPSMS=0"));

      this->printlnFlush(F("AT+CSCON=0"));

      this->printlnFlush(F("AT+CEDRXS=0,5"));

      this->printlnFlush(F("AT+QMTCLOSE=0"));

      this->printlnFlush(F("AT+QMTDISC=0"));

      this->connState = STATE_WAITING_SETUP;
    }

    if (this->connState == STATE_FINISH_SETUP) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nGETTING IMEI"));

        this->printlnFlush(F("AT+CGSN=1"));

        this->connState = STATE_WAITING_IMEI;
      }
    }

    if (this->connState == STATE_FINISH_IMEI) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nGETTING CSQ"));

        this->printlnFlush(F("AT+CSQ"));

        this->connState = STATE_WAITING_CSQ;
      }
    }

    if (this->connState == STATE_FINISH_CSQ) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nGETTING CGATT"));

        this->printlnFlush(F("AT+CGATT?"));

        this->connState = STATE_WAITING_CGATT;
      }
    }

    if (this->connState == STATE_FINISH_CGATT) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nGETTING CEREG"));

        this->printlnFlush(F("AT+CEREG?"));

        this->connState = STATE_WAITING_CEREG;
      }
    }

    if (this->connState == STATE_FINISH_CEREG) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nOPENING MQTT..."));

        this->printlnFlush(F("AT+QMTOPEN=0,8.210.84.24,1880"));

        this->connState = STATE_WAITING_OPEN;
      }
    }

    if (this->connState == STATE_FINISH_OPEN) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println(F("\r\nCONNECTING MQTT..."));

        this->printlnFlush(nbIotConnCmd);
        this->connState = STATE_WAITING_CONN;
      }
    }

    if (this->connState == STATE_FINISH_CONN) {
      Serial.println(F("\r\nSUBSCRBING TOPIC..."));

      this->printlnFlush(nbiotSubsCmd);
      this->connState = STATE_WAITING_SUB;
    }

    if (this->connState == STATE_FINISH_SUB) {
      this->finishInit = true;
      this->connState = STATE_FINISH_NBIOT_INIT;
      Serial.println(F("\r\nFINISH INIT NBIOT"));
    }


    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      if (this->pipelineState == PIPELINE_DEFAULT) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.println(F("\r\nQUERYING CSQ"));
          this->printlnFlush(F("AT+CSQ"));

          this->pipelineState = PIPELINE_WAITING_CSQ;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CSQ) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.println(F("\r\nQUERYING CGATT"));
          this->printlnFlush(F("AT+CGATT?"));

          this->pipelineState = PIPELINE_WAITING_CGATT;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CGATT) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.println(F("\r\nQUERYING CEREG"));
          this->printlnFlush(F("AT+CEREG?"));

          this->pipelineState = PIPELINE_WAITING_CEREG;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CEREG) {
        if (nbiotTimer.autoExpired(13000)) {
          Serial.println(F("\r\nEXECUTE REGULAR PUBLISH"));

          this->printlnFlush(nbiotPubMsgPrepare);
          this->pubMsgPayloadLock = true;  // disable the preparation of payload
          this->pipelineState = PIPELINE_WAITING_PREPARE_PUBMSG;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_PREPARE_PUBMSG) {
        if (nbiotTimer.autoExpired(2000)) {
          this->printlnFlush(nbiotPubMsgPayload);
          this->pubMsgPayloadLock = false;  // release the lock
          this->pipelineState = PIPELINE_WAITING_PUBLISH;
        }
      }
    }
  }

  void listen() {
    if (NBIoTSerial.available() > 0) {
      while (NBIoTSerial.available() > 0) {
        char c = NBIoTSerial.read();

        if (this->debugMode) {
          Serial.print(c);
        }

        if (c != '\r' && c != '\n') {
          nbiotSerialRecv += c;
        }

        if (c == '\r') {
          this->answer();
          this->handleReadMsg();
          this->clearResBuffer();
        }
      }
    }

    if (nbiotSoftReset) {
      this->resetBuffers();
      nbiotSoftReset = false;
      this->connState = STATE_WAITING_RESET;
      this->pipelineState = PIPELINE_DEFAULT;
      Serial.println(F("[SOFT_RESET]"));
    }
  }

  void answer() {
    int idx = -1;

    if (this->connState == STATE_WAITING_IP) {
      cmpStr = F("+IP:");
      idx = nbiotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nFINISH WAITING IP"));

        this->connState = STATE_FINISH_IP;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_SETUP) {
      {
        Serial.println(F("\r\nFINISH SETUP"));

        this->connState = STATE_FINISH_SETUP;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_IMEI) {
      cmpStr = F("+CGSN:");
      idx = nbiotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nFINISH GETTING IMEI"));

        this->connState = STATE_FINISH_IMEI;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CSQ) {
      cmpStr = F("+CSQ:");
      idx = nbiotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nFINISH GETTING CSQ"));

        this->connState = STATE_FINISH_CSQ;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CGATT) {
      cmpStr = F("+CGATT:");
      idx = nbiotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nFINISH GETTING CGATT"));

        this->connState = STATE_FINISH_CGATT;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CEREG) {
      cmpStr = F("+CEREG:");
      idx = nbiotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nFINISH GETTING CEREG"));

        this->connState = STATE_FINISH_CEREG;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_OPEN) {
      cmpStr = F("+QMTOPEN: 0,0");
      idx = nbiotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nOPENED MQTT"));

        this->connState = STATE_FINISH_OPEN;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CONN) {
      cmpStr = F("+QMTCONN: 0,0,0");
      idx = nbiotSerialRecv.indexOf(cmpStr);
      if (idx > -1) {
        Serial.println(F("\r\nCONNECTED MQTT"));

        this->connState = STATE_FINISH_CONN;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_SUB) {
      cmpStr = F("+QMTSUB:");
      idx = nbiotSerialRecv.indexOf(cmpStr);

      if (idx > -1) {
        Serial.println(F("\r\nSUB TOPIC OK"));

        this->connState = STATE_FINISH_SUB;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      int idx = -1;
      if (this->pipelineState == PIPELINE_WAITING_CSQ) {

        cmpStr = F("+CSQ:");
        idx = nbiotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          Serial.println(F("\r\nFINISH GETTING CSQ"));

          this->pipelineState = PIPELINE_FINISH_CSQ;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CGATT) {

        cmpStr = F("+CGATT:");
        idx = nbiotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          Serial.println(F("\r\nFINISH GETTING CGATT"));

          this->pipelineState = PIPELINE_FINISH_CGATT;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CEREG) {

        cmpStr = F("+CEREG:");
        idx = nbiotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          Serial.println(F("\r\nFINISH GETTING CEREG"));

          this->pipelineState = PIPELINE_FINISH_CEREG;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PREPARE_PUBMSG) {

        cmpStr = F(">");
        idx = nbiotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          this->pipelineState = PIPELINE_FINISH_PREPARE_PUBMSG;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PUBLISH) {

        cmpStr = F("+QMTPUB: 0,0,0");
        idx = nbiotSerialRecv.indexOf(cmpStr);

        if (idx > -1) {
          this->pipelineState = PIPELINE_DEFAULT;
          Serial.println(F("\r\nFINISH REGULAR PUBLISH"));

          nbiotWatchdog.pet();
        }
      }
    }
  }

  void handleReadMsg() {
    int idx = -1;

    // === handle IMEI ===

    cmpStr = F("+CGSN:");
    idx = nbiotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      {
        nbiotIMEI = nbiotSerialRecv.substring(7, 7 + 15);
      }
      if (!utils.isNumeric(nbiotIMEI)) {
        nbiotSoftReset = true;
      }

      nbIotConnCmd = F("AT+QMTCONN=0,dev_");
      nbIotConnCmd.concat(nbiotIMEI);
      nbIotConnCmd.concat(F(",tswh,1Wo=[6vA0m"));

      nbiotSubsCmd = F("AT+QMTSUB=0,1,rgt/");
      nbiotSubsCmd.concat(nbiotIMEI);
      nbiotSubsCmd.concat(F("/out,0"));
    }

    // === handle CGATT ===
    cmpStr = F("+CGATT:");
    idx = nbiotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      {
        nbiotCGATT = nbiotSerialRecv.substring(8, 8 + 1);
      }

      if (nbiotCGATT != "1") {
        nbiotSoftReset = true;
      }
    }

    // === handle CEREG ===

    cmpStr = F("+CEREG:");
    idx = nbiotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      {
        nbiotCEREG = nbiotSerialRecv.substring(8, 8 + 3);
      }

      if (nbiotCEREG != "0,1") {
        nbiotSoftReset = true;
      }
    }

    // === handle CSQ ===

    cmpStr = F("+CSQ:");
    idx = nbiotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      cmpStr = F(": ");
      int winStart = nbiotSerialRecv.indexOf(cmpStr);
      cmpStr = F(",");
      int winEnd = nbiotSerialRecv.indexOf(cmpStr);

      {
        nbiotCSQ = nbiotSerialRecv.substring(winStart + 2, winEnd);
      }

      if (nbiotCSQ == "99") {
        nbiotSoftReset = true;
      }

      if (!utils.isNumeric(nbiotCSQ)) {
        nbiotSoftReset = true;
        nbiotCSQ = F("0");
      }

      {
        int numCSQ = nbiotCSQ.toInt();
        if (!(numCSQ >= 5 && numCSQ <= 31)) {
          nbiotSoftReset = true;
          nbiotCSQ = F("0");
        }
      }
    }

    // === handle publish ACK ===
    cmpStr = F("+QMTPUB:");
    idx = nbiotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {

      {
        nbiotPubAck = nbiotSerialRecv.substring(9, 9 + 5);
      }

      if (nbiotPubAck != "0,0,0") {
        nbiotSoftReset = true;
      }
    }

    // === handle SUB ACK ===
    cmpStr = F("+QMTSUB:");
    idx = nbiotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {

      {
        nbiotSubAck = nbiotSerialRecv.substring(9, 9 + 7);
      }

      if (nbiotSubAck != "0,1,0,0") {
        nbiotSoftReset = true;
      }
    }

    // === handle SUB received msg and parse it's content ===
    cmpStr = F("+QMTRECV:");
    idx = nbiotSerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      {
        nbiotSubMsgContent = nbiotSerialRecv.substring(41, 46);
      }
      Serial.println(nbiotSubMsgContent);
    }
  }

  void forcePublish() {
    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      if (this->pipelineState == PIPELINE_WAITING_PREPARE_PUBMSG
          || this->pipelineState == PIPELINE_FINISH_PREPARE_PUBMSG
          || this->pipelineState == PIPELINE_WAITING_PUBLISH
          || this->pipelineState == PIPELINE_FINISH_PUBLISH) {
        return;
      } else {
        Serial.println(F("\r\nFORCE PUBLISH STOPPED"));

        this->printlnFlush(nbiotPubMsgCommand);
      }
    }
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif