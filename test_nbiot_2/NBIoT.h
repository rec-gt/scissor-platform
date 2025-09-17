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

  String serialRecv = "";
  String connCommand = "";
  String subsCommand = "";

  void clearSerialBuffer() {
    while (NBIoTSerial.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    serialRecvNBIoT = "";
  }

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
    NBIoTSerial.println(cmd);
    NBIoTSerial.flush();
    delay(delayTime);
  }

public:
  NBIOT_STATE connState = STATE_WAITING_RESET;
  PUBSUB_PIPELINE pipelineState = PIPELINE_DEFAULT;

  // for monitoring
  String IP = "";
  String CSQ = "";
  String IMEI = "";
  String CGATT = "";
  String CEREG = "";

  // for publish
  String pubMsgPayload = "";
  String pubMsgPrepare = "";
  String pubMsgCommand = "";

  // important, for pubMsgCommand consistency due to the async concat property of "pubMsgPrepare" & "pubMsgPayload"
  bool pubMsgPayloadLock = false;

  // for subscribe
  String subMsgContent = "";

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

  void setup() {
    NBIoTSerial.begin(9600);

    // serialRecvNBIoT.reserve(32);
    // this->connCommand.reserve(39);
    // this->subsCommand.reserve(128);

    // this->IP.reserve(16);
    // this->CSQ.reserve(8);
    // this->IMEI.reserve(32);
    // this->CGATT.reserve(8);
    // this->CEREG.reserve(8);

    // this->pubMsgPayload.reserve(64);
    // this->pubMsgPrepare.reserve(64);
    // this->pubMsgCommand.reserve(128);
    // this->subMsgContent.reserve(32);
  }

  void init(bool asyncInitMode = false) {
    // Serial.println("=== NBIOT START ===");
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
      serialRecvNBIoT = "";
      this->connCommand = "";
      this->subsCommand = "";

      this->IP = "";
      this->CSQ = "";
      this->IMEI = "";
      this->CGATT = "";
      this->CEREG = "";

      this->pubMsgPayload = "";
      this->pubMsgPrepare = "";
      this->pubMsgCommand = "";

      digitalWrite(this->resetPin, LOW);
      if (nbiotTimer.autoExpired(1000)) {
        digitalWrite(this->resetPin, HIGH);
        this->connState = STATE_FINISH_RESET;
        delay(100);
      }
    }

    if (this->connState == STATE_FINISH_RESET) {
      if (nbiotTimer.autoExpired(1000)) {
        // Serial.println("WAITING IP");
        this->printlnFlush("AT+QSCLK=0");
        this->connState = STATE_WAITING_IP;
      }
    }

    if (this->connState == STATE_FINISH_IP) {
      // Serial.println("SETTING UP NBIOT");
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
        // Serial.println("GETTING IMEI");
        this->printlnFlush("AT+CGSN=1");
        this->connState = STATE_WAITING_IMEI;
      }
    }

    if (this->connState == STATE_FINISH_IMEI) {
      if (nbiotTimer.autoExpired(1000UL)) {
        // Serial.println("GETTING CSQ");
        this->printlnFlush("AT+CSQ");
        this->connState = STATE_WAITING_CSQ;
      }
    }

    if (this->connState == STATE_FINISH_CSQ) {
      if (nbiotTimer.autoExpired(1000UL)) {
        // Serial.println("GETTING CGATT");
        this->printlnFlush("AT+CGATT?");
        this->connState = STATE_WAITING_CGATT;
      }
    }

    if (this->connState == STATE_FINISH_CGATT) {
      if (nbiotTimer.autoExpired(1000UL)) {
        // Serial.println("GETTING CEREG");
        this->printlnFlush("AT+CEREG?");
        this->connState = STATE_WAITING_CEREG;
      }
    }

    if (this->connState == STATE_FINISH_CEREG) {
      if (nbiotTimer.autoExpired(1000UL)) {
        // Serial.println("OPENING MQTT...");
        this->printlnFlush("AT+QMTOPEN=0,8.210.84.24,1880");
        this->connState = STATE_WAITING_OPEN;
      }
    }

    if (this->connState == STATE_FINISH_OPEN) {
      if (nbiotTimer.autoExpired(1000UL)) {
        // Serial.println("CONNECTING MQTT...");
        this->printlnFlush(this->connCommand);
        this->connState = STATE_WAITING_CONN;
      }
    }

    if (this->connState == STATE_FINISH_CONN) {
      // Serial.println("SUBSCRBING TOPIC...");
      this->printlnFlush(this->subsCommand);
      this->connState = STATE_WAITING_SUB;
    }

    if (this->connState == STATE_FINISH_SUB) {
      this->finishInit = true;
      this->connState = STATE_FINISH_NBIOT_INIT;
      // Serial.println("FINISH INIT NBIOT");
    }

    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      if (this->pipelineState == PIPELINE_DEFAULT) {
        if (nbiotTimer.autoExpired(5000)) {
          // Serial.println("QUERYING CSQ");
          this->printlnFlush("AT+CSQ");
          this->pipelineState = PIPELINE_WAITING_CSQ;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CSQ) {
        if (nbiotTimer.autoExpired(5000)) {
          // Serial.println("QUERYING CGATT");
          this->printlnFlush("AT+CGATT?");
          this->pipelineState = PIPELINE_WAITING_CGATT;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CGATT) {
        if (nbiotTimer.autoExpired(5000)) {
          // Serial.println("QUERYING CEREG");
          this->printlnFlush("AT+CEREG?");
          this->pipelineState = PIPELINE_WAITING_CEREG;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_CEREG) {
        if (nbiotTimer.autoExpired(13000)) {
          // Serial.println("EXECUTE REGULAR PUBLISH");
          this->printlnFlush(this->pubMsgPrepare);
          this->pubMsgPayloadLock = true;  // disable the preparation of payload
          this->pipelineState = PIPELINE_WAITING_PREPARE_PUBMSG;
        }
      }

      if (this->pipelineState == PIPELINE_FINISH_PREPARE_PUBMSG) {
        if (nbiotTimer.autoExpired(2000)) {
          this->printlnFlush(this->pubMsgPayload);
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
          serialRecvNBIoT += c;
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
      // Serial.println("[SOFT_RESET]");
    }
  }

  void answer() {
    int idx = -1;

    if (this->connState == STATE_WAITING_IP) {
      idx = serialRecvNBIoT.indexOf("+IP:");
      if (idx > -1) {
        // Serial.println("FINISH WAITING IP");
        this->connState = STATE_FINISH_IP;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_SETUP) {
      {
        // Serial.println("FINISH SETUP");
        this->connState = STATE_FINISH_SETUP;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_IMEI) {
      idx = serialRecvNBIoT.indexOf("+CGSN:");
      if (idx > -1) {
        // Serial.println("FINISH GETTING IMEI");
        this->connState = STATE_FINISH_IMEI;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CSQ) {
      idx = serialRecvNBIoT.indexOf("+CSQ:");
      if (idx > -1) {
        // Serial.println("FINISH GETTING CSQ");
        this->connState = STATE_FINISH_CSQ;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CGATT) {
      idx = serialRecvNBIoT.indexOf("+CGATT:");
      if (idx > -1) {
        // Serial.println("FINISH GETTING CGATT");
        this->connState = STATE_FINISH_CGATT;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CEREG) {
      idx = serialRecvNBIoT.indexOf("+CEREG:");
      if (idx > -1) {
        // Serial.println("FINISH GETTING CEREG");
        this->connState = STATE_FINISH_CEREG;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_OPEN) {
      idx = serialRecvNBIoT.indexOf("+QMTOPEN: 0,0");
      if (idx > -1) {
        // Serial.println("OPENED MQTT");
        this->connState = STATE_FINISH_OPEN;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_CONN) {
      idx = serialRecvNBIoT.indexOf("+QMTCONN: 0,0,0");
      if (idx > -1) {
        // Serial.println("CONNECTED MQTT");
        this->connState = STATE_FINISH_CONN;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_WAITING_SUB) {
      idx = serialRecvNBIoT.indexOf("+QMTSUB:");
      if (idx > -1) {
        // Serial.println("SUB TOPIC OK");
        this->connState = STATE_FINISH_SUB;
        nbiotWatchdog.pet();
      }
    }

    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      int idx = -1;
      if (this->pipelineState == PIPELINE_WAITING_CSQ) {
        idx = serialRecvNBIoT.indexOf("+CSQ:");
        if (idx > -1) {
          // Serial.println("FINISH GETTING CSQ");
          this->pipelineState = PIPELINE_FINISH_CSQ;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CGATT) {
        idx = serialRecvNBIoT.indexOf("+CGATT:");
        if (idx > -1) {
          // Serial.println("FINISH GETTING CGATT");
          this->pipelineState = PIPELINE_FINISH_CGATT;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_CEREG) {
        idx = serialRecvNBIoT.indexOf("+CEREG:");
        if (idx > -1) {
          // Serial.println("FINISH GETTING CEREG");
          this->pipelineState = PIPELINE_FINISH_CEREG;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PREPARE_PUBMSG) {
        idx = serialRecvNBIoT.indexOf(">");
        if (idx > -1) {
          this->pipelineState = PIPELINE_FINISH_PREPARE_PUBMSG;
          nbiotWatchdog.pet();
        }
      }

      if (this->pipelineState == PIPELINE_WAITING_PUBLISH) {
        idx = serialRecvNBIoT.indexOf("+QMTPUB: 0,0,0");
        if (idx > -1) {
          this->pipelineState = PIPELINE_DEFAULT;
          // Serial.println("FINISH REGULAR PUBLISH");
          nbiotWatchdog.pet();
        }
      }
    }
  }

  void handleReadMsg() {
    int idx = -1;

    // === handle IP ===
    idx = serialRecvNBIoT.indexOf("+IP:");
    if (idx > -1) {
      this->IP = serialRecvNBIoT.substring(5, 5 + 16);
    }

    // === handle IMEI ===
    idx = serialRecvNBIoT.indexOf("+CGSN:");
    if (idx > -1) {
      this->IMEI = serialRecvNBIoT.substring(7, 7 + 15);
      if (!utils.isNumeric(this->IMEI)) {
        nbiotSoftReset = true;
      }

      this->connCommand = "AT+QMTCONN=0,dev_";
      this->connCommand.concat(this->IMEI);
      this->connCommand.concat(",tswh,1Wo=[6vA0m");

      this->subsCommand = "AT+QMTSUB=0,1,rgt/";
      this->subsCommand.concat(this->IMEI);
      this->subsCommand.concat("/out,0");
    }

    // === handle CGATT ===
    idx = serialRecvNBIoT.indexOf("+CGATT:");
    if (idx > -1) {
      this->CGATT = serialRecvNBIoT.substring(8, 8 + 1);

      if (this->CGATT != "1") {
        nbiotSoftReset = true;
      }
    }

    // === handle CEREG ===
    idx = serialRecvNBIoT.indexOf("+CEREG:");
    if (idx > -1) {
      this->CEREG = serialRecvNBIoT.substring(8, 8 + 3);

      if (this->CEREG != "0,1") {
        nbiotSoftReset = true;
      }
    }

    // === handle CSQ ===
    idx = serialRecvNBIoT.indexOf("+CSQ:");
    if (idx > -1) {
      int winStart = idx + 6;
      int winEnd = winStart + 2;
      this->CSQ = serialRecvNBIoT.substring(winStart, winEnd);

      if (this->CSQ == "99") {
        nbiotSoftReset = true;
      }

      String strNum = this->CSQ;

      if (!utils.isNumeric(strNum)) {
        nbiotSoftReset = true;
      }

      int numCSQ = strNum.toInt();
      if (!(numCSQ >= 5 && numCSQ <= 31)) {
        nbiotSoftReset = true;
      }

      this->CSQ = String(numCSQ);
    }

    // === handle publish ACK ===
    idx = serialRecvNBIoT.indexOf("+QMTPUB:");
    if (idx > -1) {
      String QMTPUB = serialRecvNBIoT.substring(9, 9 + 5);
      if (QMTPUB != "0,0,0") {
        nbiotSoftReset = true;
      }
    }

    // === handle SUB ACK ===
    idx = serialRecvNBIoT.indexOf("+QMTSUB:");
    if (idx > -1) {
      String QMTSUB = serialRecvNBIoT.substring(9, 9 + 7);
      if (QMTSUB != "0,1,0,0") {
        nbiotSoftReset = true;
      }
    }

    // === handle SUB received msg and parse it's content ===
    // idx = serialRecvNBIoT.indexOf("+QMTRECV:");
    // if (idx > -1) {
    //   int startPos = serialRecvNBIoT.indexOf("[");
    //   int endPos = serialRecvNBIoT.indexOf("]", startPos);

    //   if (startPos > -1 && endPos > -1) {
    //     this->subMsgContent = serialRecvNBIoT.substring(startPos + 1, endPos);
    //     Serial.print(this->subMsgContent);
    //   } else {
    //     this->subMsgContent = "";
    //   }
    // }

    idx = serialRecvNBIoT.indexOf("+QMTRECV:");
    if (idx > -1) {
      this->subMsgContent = serialRecvNBIoT.substring(9, 20);
      this->subMsgContent = serialRecvNBIoT.substring(41, 46);
      Serial.println(this->subMsgContent);
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
        // Serial.println("FORCE PUBLISH STOPPED");
        this->printlnFlush(this->pubMsgCommand);
      }
    }
  }

  void readRecvMsg(String& assign) {
    assign = this->subMsgContent;
    this->subMsgContent = "";
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif