#include "AsyncTimer.h"
#include "Utils.h"
#include "Watchdog.h"
#include "Globals.h"

#ifndef NBIoT_h
#define NBIoT_h

#define NBIoTSerial Serial1

Watchdog nbiot_wdt(30000UL);

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

  enum PUB_PIPELINE {
    PUB_PIPELINE_DEFAULT,
    PUB_PIPELINE_WAITING_PREPARE_PUBMSG,
    PUB_PIPELINE_FINISH_PREPARE_PUBMSG,
    PUB_PIPELINE_WAITING_PUBLISH,
    PUB_PIPELINE_FINISH_PUBLISH,
  };

  enum SUB_PIPELINE {
    SUB_PIPELINE_DEFAULT,
    SUB_PIPELINE_HANDLE_RECV_MSG,
    SUB_PIPELINE_FINISH_RECV_MSG,
  };

  bool finishInit = false;

  byte connState = STATE_WAITING_RESET;
  byte pubState = PIPELINE_DEFAULT;

  byte resetPin = 11;

  bool debug = false;

  void clearSerialBuffer() {
    while (NBIoTSerial.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    serialRes = "";
  }

public:
  String IP = "";
  String CSQ = "";
  String IMEI = "";
  String CGATT = "";
  String CEREG = "";

  NBIoT() {
    pinMode(this->resetPin, OUTPUT);
    digitalWrite(this->resetPin, HIGH);
  }

  void resetBuffers() {
    this->clearSerialBuffer();
    this->clearResBuffer();
    delay(10);
  }

  void debugMode() {
    this->debug = true;
  }

  void init() {
    Serial.print("\r\n=== NBIOT START ===\r\n");
    nbiot_wdt.enable();
    nbiot_wdt.setCallback([]() {
      nbiotSoftReset = true;
    });
    this->resetBuffers();
    this->loop();
  }

  void loop() {
    while (1) {
      nbiot_wdt.monitor();

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
      digitalWrite(this->resetPin, LOW);
      if (nbiotTimer.autoExpired(1000)) {
        digitalWrite(this->resetPin, HIGH);
        this->connState = STATE_FINISH_RESET;
        delay(100);
      }
    }

    if (this->connState == STATE_FINISH_RESET) {
      if (nbiotTimer.autoExpired(1000)) {
        Serial.print("\r\nWAITING IP\r\n");
        NBIoTSerial.println("AT+QSCLK=0");
        delay(10);
        this->connState = STATE_WAITING_IP;
      }
    }

    if (this->connState == STATE_FINISH_IP) {
      Serial.print("\r\nSETTING UP NBIOT\r\n");
      NBIoTSerial.println("AT+CFUN=1");
      delay(10);
      NBIoTSerial.println("AT+QSCLK=0");
      delay(10);
      NBIoTSerial.println("AT+CPSMS=0");
      delay(10);
      NBIoTSerial.println("AT+CSCON=0");
      delay(10);
      NBIoTSerial.println("AT+CEDRXS=0,5");
      delay(10);
      NBIoTSerial.println("AT+QMTCLOSE=0");
      delay(10);
      NBIoTSerial.println("AT+QMTDISC=0");
      delay(10);
      this->connState = STATE_WAITING_SETUP;
    }

    if (this->connState == STATE_FINISH_SETUP) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nGETTING IMEI\r\n");
        NBIoTSerial.println("AT+CGSN=1");
        this->connState = STATE_WAITING_IMEI;
      }
    }

    if (this->connState == STATE_FINISH_IMEI) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nGETTING CSQ\r\n");
        NBIoTSerial.println("AT+CSQ");
        this->connState = STATE_WAITING_CSQ;
      }
    }

    if (this->connState == STATE_FINISH_CSQ) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nGETTING CGATT\r\n");
        NBIoTSerial.println("AT+CGATT?");
        this->connState = STATE_WAITING_CGATT;
      }
    }

    if (this->connState == STATE_FINISH_CGATT) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nGETTING CEREG\r\n");
        NBIoTSerial.println("AT+CEREG?");
        this->connState = STATE_WAITING_CEREG;
      }
    }

    if (this->connState == STATE_FINISH_CEREG) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nOPENING MQTT...\r\n");
        NBIoTSerial.println("AT+QMTOPEN=0,8.210.84.24,1880");
        this->connState = STATE_WAITING_OPEN;
      }
    }

    if (this->connState == STATE_FINISH_OPEN) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nCONNECTING MQTT...\r\n");
        NBIoTSerial.println(connStr);
        NBIoTSerial.flush();
        this->connState = STATE_WAITING_CONN;
      }
    }

    if (this->connState == STATE_FINISH_CONN) {
      Serial.print("\r\nSUBSCRBING TOPIC...\r\n");
      NBIoTSerial.println(subStr);
      NBIoTSerial.flush();
      this->connState = STATE_WAITING_SUB;
    }

    if (this->connState == STATE_FINISH_SUB) {
      this->finishInit = true;
      this->connState = STATE_FINISH_NBIOT_INIT;
    }

    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      if (this->pubState == PIPELINE_DEFAULT) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.print("\r\nQUERYING CSQ\r\n");
          NBIoTSerial.println("AT+CSQ");
          this->pubState = PIPELINE_WAITING_CSQ;
        }
      }

      if (this->pubState == PIPELINE_FINISH_CSQ) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.print("\r\nQUERYING CGATT\r\n");
          NBIoTSerial.println("AT+CGATT?");
          this->pubState = PIPELINE_WAITING_CGATT;
        }
      }

      if (this->pubState == PIPELINE_FINISH_CGATT) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.print("\r\nQUERYING CEREG\r\n");
          NBIoTSerial.println("AT+CEREG?");
          this->pubState = PIPELINE_WAITING_CEREG;
        }
      }

      if (this->pubState == PIPELINE_FINISH_CEREG) {
        if (nbiotTimer.autoExpired(13000)) {
          Serial.print("\r\nEXECUTE REGULAR PUBLISH\r\n");
          NBIoTSerial.println(pubMsgPrepare);
          NBIoTSerial.flush();
          this->pubState = PIPELINE_WAITING_PREPARE_PUBMSG;
        }
      }

      if (this->pubState == PIPELINE_FINISH_PREPARE_PUBMSG) {
        if (nbiotTimer.autoExpired(2000)) {
          NBIoTSerial.println(pubMsgContent);
          NBIoTSerial.flush();
          this->pubState = PIPELINE_WAITING_PUBLISH;
        }
      }
    }
  }

  void listen() {
    if (NBIoTSerial.available() > 0) {
      while (NBIoTSerial.available() > 0) {
        char _byte = NBIoTSerial.read();

        if (this->debug) {
          Serial.print(_byte);
        }

        if (_byte != '\r' && _byte != '\n') {
          serialRes += _byte;
        }

        if (_byte == '\r') {
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
      this->pubState = PIPELINE_DEFAULT;
      Serial.print("\r\n[SOFT_RESET]\r\n");
    }
  }

  void answer() {
    int idx = -1;

    if (this->connState == STATE_WAITING_IP) {
      idx = serialRes.indexOf("+IP:");
      if (idx > -1) {
        Serial.print("\r\nFINISH WAITING IP\r\n");
        this->connState = STATE_FINISH_IP;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_SETUP) {
      {
        Serial.print("\r\nFINISH SETUP\r\n");
        this->connState = STATE_FINISH_SETUP;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_IMEI) {
      idx = serialRes.indexOf("+CGSN:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING IMEI\r\n");
        this->connState = STATE_FINISH_IMEI;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CSQ) {
      idx = serialRes.indexOf("+CSQ:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING CSQ\r\n");
        this->connState = STATE_FINISH_CSQ;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CGATT) {
      idx = serialRes.indexOf("+CGATT:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING CGATT\r\n");
        this->connState = STATE_FINISH_CGATT;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CEREG) {
      idx = serialRes.indexOf("+CEREG:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING CEREG\r\n");
        this->connState = STATE_FINISH_CEREG;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_OPEN) {
      idx = serialRes.indexOf("+QMTOPEN: 0,0");
      if (idx > -1) {
        Serial.print("\r\nOPENED MQTT\r\n");
        this->connState = STATE_FINISH_OPEN;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CONN) {
      idx = serialRes.indexOf("+QMTCONN: 0,0,0");
      if (idx > -1) {
        Serial.print("\r\nCONNECTED MQTT\r\n");
        this->connState = STATE_FINISH_CONN;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_SUB) {
      idx = serialRes.indexOf("+QMTSUB: 0,1,0,0");
      if (idx > -1) {
        Serial.print("\r\nSUB TOPIC OK\r\n");
        this->connState = STATE_FINISH_SUB;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      int idx = -1;
      if (this->pubState == PIPELINE_WAITING_CSQ) {
        idx = serialRes.indexOf("+CSQ:");
        if (idx > -1) {
          Serial.print("\r\nFINISH GETTING CSQ\r\n");
          this->pubState = PIPELINE_FINISH_CSQ;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_CGATT) {
        idx = serialRes.indexOf("+CGATT:");
        if (idx > -1) {
          Serial.print("\r\nFINISH GETTING CGATT\r\n");
          this->pubState = PIPELINE_FINISH_CGATT;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_CEREG) {
        idx = serialRes.indexOf("+CEREG:");
        if (idx > -1) {
          Serial.print("\r\nFINISH GETTING CEREG\r\n");
          this->pubState = PIPELINE_FINISH_CEREG;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_PREPARE_PUBMSG) {
        idx = serialRes.indexOf(">");
        if (idx > -1) {
          this->pubState = PIPELINE_FINISH_PREPARE_PUBMSG;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_PUBLISH) {
        // idx = serialRes.indexOf("+QMTPUB: 0,0,0");
        idx = serialRes.indexOf("OK");
        if (idx > -1) {
          this->pubState = PIPELINE_DEFAULT;
          Serial.print("\r\nFINISH REGULAR PUBLISH\r\n");
          nbiot_wdt.pet();
        }
      }
    }
  }

  void handleReadMsg() {
    int idx = -1;

    // === handle IP ===
    idx = serialRes.indexOf("+IP:");
    if (idx > -1) {
      this->IP = serialRes.substring(5, 5 + 16);
    }

    // === handle IMEI ===
    idx = serialRes.indexOf("+CGSN:");
    if (idx > -1) {
      this->IMEI = serialRes.substring(7, 7 + 15);

      if (!utils.isNumeric(this->IMEI)) {
        nbiotSoftReset = true;
      }

      connStr = "AT+QMTCONN=0,dev_";
      connStr.concat(this->IMEI);
      connStr.concat(",tswh,1Wo=[6vA0m");
    }

    // === handle CGATT ===
    idx = serialRes.indexOf("+CGATT:");
    if (idx > -1) {
      this->CGATT = serialRes.substring(8, 8 + 1);

      if (this->CGATT != "1") {
        nbiotSoftReset = true;
      }
    }

    // === handle CEREG ===
    idx = serialRes.indexOf("+CEREG:");
    if (idx > -1) {
      this->CEREG = serialRes.substring(8, 8 + 3);

      if (this->CEREG != "0,1") {
        nbiotSoftReset = true;
      }
    }

    // === handle CSQ ===
    idx = serialRes.indexOf("+CSQ:");
    if (idx > -1) {
      int winStart = idx + 6;
      int winEnd = winStart + 2;
      this->CSQ = serialRes.substring(winStart, winEnd);

      if (this->CSQ == "99") {
        nbiotSoftReset = true;
      }

      String strNum = this->CSQ;

      if (!utils.isNumeric(strNum)) {
        nbiotSoftReset = true;
      }

      int numCSQ = strNum.toInt();
      if (!(numCSQ >= 8 && numCSQ <= 31)) {
        nbiotSoftReset = true;
      }

      this->CSQ = String(numCSQ);
    }

    // === handle publish ACK ===
    idx = serialRes.indexOf("+QMTPUB:");
    if (idx > -1) {
      String QMTPUB = serialRes.substring(9, 9 + 5);
      if (QMTPUB != "0,0,0") {
        nbiotSoftReset = true;
      }
    }

    // === handle SUB ACK ===
    idx = serialRes.indexOf("+QMTSUB:");
    if (idx > -1) {
      String QMTSUB = serialRes.substring(9, 9 + 7);
      if (QMTSUB != "0,1,0,0") {
        nbiotSoftReset = true;
      }
    }

    // === handle SUB received msg and parse it's content ===
    idx = serialRes.indexOf("+QMTRECV:");
    if (idx > -1) {
      String QMTSUB = serialRes.substring(9, 9 + 7);
      if (QMTSUB != "0,1,0,0") {
        nbiotSoftReset = true;
      }
    }
  }



  void forcePublish() {
    if (this->connState == STATE_FINISH_NBIOT_INIT) {
      if (this->pubState == PIPELINE_WAITING_PREPARE_PUBMSG
          || this->pubState == PIPELINE_FINISH_PREPARE_PUBMSG
          || this->pubState == PIPELINE_WAITING_PUBLISH
          || this->pubState == PIPELINE_FINISH_PUBLISH) {
        return;
      } else {
        Serial.print("\r\nFORCE PUBLISH STOPPED\r\n");
        NBIoTSerial.println(pubMsgForce);
        NBIoTSerial.flush();
      }
    }
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif