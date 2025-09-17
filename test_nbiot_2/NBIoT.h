#include "AsyncTimer.h"
#include "Utils.h"
#include "Watchdog.h"
#include "Globals.h"

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

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
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
    Serial.println("\r\n=== NBIOT START ===");
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
  }

  void ask() {
    if (this->connState == STATE_WAITING_RESET) {
      nbiotSerialRecv = "";
      nbIotConnCmd = "";
      nbiotSubsCmd = "";

      // nbiotIP = "";
      nbiotCSQ = "";
      nbiotIMEI = "";
      nbiotCGATT = "";
      nbiotCEREG = "";

      nbiotPubMsgPayload = "";
      nbiotPubMsgPrepare = "";
      nbiotPubMsgCommand = "";

      digitalWrite(this->resetPin, LOW);
      if (nbiotTimer.autoExpired(1000)) {
        digitalWrite(this->resetPin, HIGH);
        this->connState = STATE_FINISH_RESET;
        delay(100);
      }
    }

    if (this->connState == STATE_FINISH_RESET) {
      if (nbiotTimer.autoExpired(1000)) {
        Serial.println("\r\nWAITING IP");
        this->printlnFlush("AT+QSCLK=0");
        this->connState = STATE_WAITING_IP;
      }
    }

    if (this->connState == STATE_FINISH_IP) {
      Serial.println("\r\nSETTING UP NBIOT");
      this->printlnFlush("AT+CFUN=1");
      this->printlnFlush("AT+QSCLK=0");
      this->printlnFlush("AT+CPSMS=0");
      this->printlnFlush("AT+CSCON=0");
      this->printlnFlush("AT+CEDRXS=0,5");
      this->printlnFlush("AT+QMTCLOSE=0");
      this->printlnFlush("AT+QMTDISC=0");
      this->connState = STATE_WAITING_SETUP;
    }

    if (this->connState == STATE_FINISH_SETUP) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println("\r\nGETTING IMEI");
        this->printlnFlush("AT+CGSN=1");
        this->connState = STATE_WAITING_IMEI;
      }
    }

    if (this->connState == STATE_FINISH_IMEI) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println("\r\nGETTING CSQ");
        this->printlnFlush("AT+CSQ");
        this->connState = STATE_WAITING_CSQ;
      }
    }

    if (this->connState == STATE_FINISH_CSQ) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println("\r\nGETTING CGATT");
        this->printlnFlush("AT+CGATT?");
        this->connState = STATE_WAITING_CGATT;
      }
    }

    if (this->connState == STATE_FINISH_CGATT) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println("\r\nGETTING CEREG");
        this->printlnFlush("AT+CEREG?");
        this->connState = STATE_WAITING_CEREG;
      }
    }

    if (this->connState == STATE_FINISH_CEREG) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println("\r\nOPENING MQTT...");
        this->printlnFlush("AT+QMTOPEN=0,8.210.84.24,1880");
        this->connState = STATE_WAITING_OPEN;
      }
    }

    if (this->connState == STATE_FINISH_OPEN) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.println("\r\nCONNECTING MQTT...");
        this->printlnFlush(nbIotConnCmd);
        this->connState = STATE_WAITING_CONN;
      }
    }

    if (this->connState == STATE_FINISH_CONN) {
      Serial.println("\r\nSUBSCRBING TOPIC...");
      this->printlnFlush(nbiotSubsCmd);
      this->connState = STATE_WAITING_SUB;
    }

    if (this->connState == STATE_FINISH_SUB) {
      this->finishInit = true;
      this->connState = STATE_FINISH_NBIOT_INIT;
      Serial.println("\r\nFINISH INIT NBIOT");
    }

    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      if (this->pipelineState == PIPELINE_DEFAULT) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.println("\r\nQUERYING CSQ");
          this->printlnFlush("AT+CSQ");
          this->pipelineState = PIPELINE_WAITING_CSQ;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CSQ) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.println("\r\nQUERYING CGATT");
          this->printlnFlush("AT+CGATT?");
          this->pipelineState = PIPELINE_WAITING_CGATT;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CGATT) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.println("\r\nQUERYING CEREG");
          this->printlnFlush("AT+CEREG?");
          this->pipelineState = PIPELINE_WAITING_CEREG;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CEREG) {
        if (nbiotTimer.autoExpired(13000)) {
          Serial.println("\r\nEXECUTE REGULAR PUBLISH");
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
      Serial.println("[SOFT_RESET]");
    }
  }

  void answer() {
    int idx = -1;

    if (this->connState == STATE_WAITING_IP) {
      idx = nbiotSerialRecv.indexOf("+IP:");
      if (idx > -1) {
        Serial.println("\r\nFINISH WAITING IP");
        this->connState = STATE_FINISH_IP;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_SETUP) {
      {
        Serial.println("\r\nFINISH SETUP");
        this->connState = STATE_FINISH_SETUP;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_IMEI) {
      idx = nbiotSerialRecv.indexOf("+CGSN:");
      if (idx > -1) {
        Serial.println("\r\nFINISH GETTING IMEI");
        this->connState = STATE_FINISH_IMEI;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CSQ) {
      idx = nbiotSerialRecv.indexOf("+CSQ:");
      if (idx > -1) {
        Serial.println("\r\nFINISH GETTING CSQ");
        this->connState = STATE_FINISH_CSQ;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CGATT) {
      idx = nbiotSerialRecv.indexOf("+CGATT:");
      if (idx > -1) {
        Serial.println("\r\nFINISH GETTING CGATT");
        this->connState = STATE_FINISH_CGATT;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CEREG) {
      idx = nbiotSerialRecv.indexOf("+CEREG:");
      if (idx > -1) {
        Serial.println("\r\nFINISH GETTING CEREG");
        this->connState = STATE_FINISH_CEREG;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_OPEN) {
      idx = nbiotSerialRecv.indexOf("+QMTOPEN: 0,0");
      if (idx > -1) {
        Serial.println("\r\nOPENED MQTT");
        this->connState = STATE_FINISH_OPEN;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CONN) {
      idx = nbiotSerialRecv.indexOf("+QMTCONN: 0,0,0");
      if (idx > -1) {
        Serial.println("\r\nCONNECTED MQTT");
        this->connState = STATE_FINISH_CONN;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_SUB) {
      idx = nbiotSerialRecv.indexOf("+QMTSUB:");
      if (idx > -1) {
        Serial.println("\r\nSUB TOPIC OK");
        this->connState = STATE_FINISH_SUB;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      int idx = -1;
      if (this->pipelineState == PIPELINE_WAITING_CSQ) {
        idx = nbiotSerialRecv.indexOf("+CSQ:");
        if (idx > -1) {
          Serial.println("\r\nFINISH GETTING CSQ");
          this->pipelineState = PIPELINE_FINISH_CSQ;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CGATT) {
        idx = nbiotSerialRecv.indexOf("+CGATT:");
        if (idx > -1) {
          Serial.println("\r\nFINISH GETTING CGATT");
          this->pipelineState = PIPELINE_FINISH_CGATT;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CEREG) {
        idx = nbiotSerialRecv.indexOf("+CEREG:");
        if (idx > -1) {
          Serial.println("\r\nFINISH GETTING CEREG");
          this->pipelineState = PIPELINE_FINISH_CEREG;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PREPARE_PUBMSG) {
        idx = nbiotSerialRecv.indexOf(">");
        if (idx > -1) {
          this->pipelineState = PIPELINE_FINISH_PREPARE_PUBMSG;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PUBLISH) {
        idx = nbiotSerialRecv.indexOf("+QMTPUB: 0,0,0");
        if (idx > -1) {
          this->pipelineState = PIPELINE_DEFAULT;
          Serial.println("\r\nFINISH REGULAR PUBLISH");
          nbiotWatchdog.pet();
        }
      }
    }
  }

  void handleReadMsg() {
    int idx = -1;

    // // === handle IP ===
    // idx = nbiotSerialRecv.indexOf("+IP:");
    // if (idx > -1) {
    //   nbiotIP = nbiotSerialRecv.substring(5, 5 + 16);
    // }

    // === handle IMEI ===
    idx = nbiotSerialRecv.indexOf("+CGSN:");
    if (idx > -1) {
      nbiotIMEI = nbiotSerialRecv.substring(7, 7 + 15);
      if (!utils.isNumeric(nbiotIMEI)) {
        nbiotSoftReset = true;
      }

      nbIotConnCmd = "AT+QMTCONN=0,dev_";
      nbIotConnCmd.concat(nbiotIMEI);
      nbIotConnCmd.concat(",tswh,1Wo=[6vA0m");

      nbiotSubsCmd = "AT+QMTSUB=0,1,rgt/";
      nbiotSubsCmd.concat(nbiotIMEI);
      nbiotSubsCmd.concat("/out,0");
    }

    // === handle CGATT ===
    idx = nbiotSerialRecv.indexOf("+CGATT:");
    if (idx > -1) {
      nbiotCGATT = nbiotSerialRecv.substring(8, 8 + 1);

      if (nbiotCGATT != "1") {
        nbiotSoftReset = true;
      }
    }

    // === handle CEREG ===
    idx = nbiotSerialRecv.indexOf("+CEREG:");
    if (idx > -1) {
      nbiotCEREG = nbiotSerialRecv.substring(8, 8 + 3);

      if (nbiotCEREG != "0,1") {
        nbiotSoftReset = true;
      }
    }

    // === handle CSQ ===
    idx = nbiotSerialRecv.indexOf("+CSQ:");
    if (idx > -1) {
      int winStart = idx + 6;
      int winEnd = winStart + 2;
      nbiotCSQ = nbiotSerialRecv.substring(winStart, winEnd);

      if (nbiotCSQ == "99") {
        nbiotSoftReset = true;
      }

      if (!utils.isNumeric(nbiotCSQ)) {
        nbiotSoftReset = true;
        nbiotCSQ = "ER";
      }

      int numCSQ = nbiotCSQ.toInt();
      if (!(numCSQ >= 5 && numCSQ <= 31)) {
        nbiotSoftReset = true;
        nbiotCSQ = "ER";
      }
    }

    // === handle publish ACK ===
    idx = nbiotSerialRecv.indexOf("+QMTPUB:");
    if (idx > -1) {
      nbiotPubAck = nbiotSerialRecv.substring(9, 9 + 5);
      if (nbiotPubAck != "0,0,0") {
        nbiotSoftReset = true;
      }
    }

    // === handle SUB ACK ===
    idx = nbiotSerialRecv.indexOf("+QMTSUB:");
    if (idx > -1) {
      nbiotSubAck = nbiotSerialRecv.substring(9, 9 + 7);
      if (nbiotSubAck != "0,1,0,0") {
        nbiotSoftReset = true;
      }
    }

    // === handle SUB received msg and parse it's content ===
    idx = nbiotSerialRecv.indexOf("+QMTRECV:");
    if (idx > -1) {
      nbiotSubMsgContent = nbiotSerialRecv.substring(41, 46);
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
        Serial.println("\r\nFORCE PUBLISH STOPPED");
        this->printlnFlush(nbiotPubMsgCommand);
      }
    }
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif