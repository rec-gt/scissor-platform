#include "AsyncTimer.h"
#include "Utils.h"
#include "Watchdog.h"

#ifndef NBIoT_h
#define NBIoT_h

#define NBIOT_SERIAL Serial1

#define NBIOT_INIT 0
#define NBIOT_CAN_START 1
#define NBIOT_CAN_OPEN 2
#define NBIOT_CAN_CONN 3
#define NBIOT_CAN_PUB 4
#define NBIOT_CAN_SUB 5

Watchdog nbiot_wdt(30UL * 1000UL);

AsyncTimer timerRESET(5UL * 1000UL);
AsyncTimer timerINIT(1UL * 10UL);
AsyncTimer timerIP(10000UL);
AsyncTimer timerIMEI(10000UL);
AsyncTimer timerCSQ(10000UL);
AsyncTimer timerCGATT(10000UL);
AsyncTimer timerOPEN(10000UL);
AsyncTimer timerCONN(10000UL);
AsyncTimer timerPUB(10000UL);
AsyncTimer timerStatusCSQ(10000UL);
AsyncTimer timerStatusCGATT(10000UL);

bool softReset = false;

class NBIoT {
private:
  enum NBIOT_STATE {
    STATE_DEFAULT,
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
    STATE_WAITING_OPEN,
    STATE_FINISH_OPEN,
    STATE_WAITING_CONN,
    STATE_FINISH_CONN,
    STATE_WAITING_PUBSUB,
  };

  enum QUERY_STATE {
    STATE_QUERY_DEFAULT,
    STATE_QUERY_STAT_CSQ,
    STATE_QUERY_STAT_CGATT,
  };

  bool finishInit = false;

  byte connState = STATE_DEFAULT;
  byte pubState = STATE_QUERY_DEFAULT;

  String res = "";

  unsigned long waitDataMillis = 0;

  byte resetPin = 11;
  bool csqReq = false;
  bool cgattReq = false;

  void clearSerialBuffer() {
    while (NBIOT_SERIAL.read() > 0) {};
  }

  void clearResBuffer() {
    this->res = "";
  }

public:

  String CSQ = "";
  String IMEI = "";
  // String IMEI = "861096060571706";
  String CGATT = "";

  NBIoT() {
    pinMode(this->resetPin, OUTPUT);
    digitalWrite(this->resetPin, LOW);
  }

  void resetHardware() {
    digitalWrite(this->resetPin, HIGH);
    if (timerRESET.autoExpired(800UL)) {
      digitalWrite(this->resetPin, LOW);
    }
  }

  void resetBuffer() {
    this->clearSerialBuffer();
    this->clearResBuffer();
  }

  void init() {
    Serial.print("\r\n[NBIOT START]\r\n");
    nbiot_wdt.enable();
    nbiot_wdt.setCallback([]() {
      softReset = true;
    });
    this->resetBuffer();
    this->loop();
  }

  void loop() {
    while (1) {
      nbiot_wdt.monitor();
      if (softReset) {
        softReset = false;
        this->connState = STATE_DEFAULT;
        Serial.print("[SOFT_RESET]");
      }

      this->ask();
      this->listen();
      if (this->finishInit) {
        break;
      } else {
        delay(1);
      }
    }

    this->publish();
  }

  void ask() {
    if (this->connState == STATE_DEFAULT) {
      this->resetHardware();
      if (timerINIT.autoExpired(1000UL)) {
        NBIOT_SERIAL.println("AT+QSCLK=0");
        delay(10);
        NBIOT_SERIAL.println("AT+CFUN=1");
        delay(10);
        NBIOT_SERIAL.println("AT+QRST=1");
        delay(10);
        NBIOT_SERIAL.println("AT+QSCLK=0");
        delay(10);
        NBIOT_SERIAL.println("AT+CFUN=1");
        delay(10);
        this->connState = STATE_WAITING_IP;
      }
    }

    if (this->connState == STATE_FINISH_IP) {
      NBIOT_SERIAL.println("AT+QSCLK=0");
      delay(10);
      NBIOT_SERIAL.println("AT+QSCLK=0");
      delay(10);
      NBIOT_SERIAL.println("AT+CFUN=1");
      delay(10);
      NBIOT_SERIAL.println("AT+CPSMS=0");
      delay(10);
      NBIOT_SERIAL.println("AT+CSCON=0");
      delay(10);
      NBIOT_SERIAL.println("AT+CEDRXS=0,5");
      delay(10);
      NBIOT_SERIAL.println("AT+QMTCLOSE=0");
      delay(10);
      NBIOT_SERIAL.println("AT+QMTDISC=0");
      delay(10);
      this->connState = STATE_WAITING_SETUP;
    }

    if (this->connState == STATE_FINISH_SETUP) {
      if (timerIMEI.autoExpired(1000UL)) {
        NBIOT_SERIAL.println("AT+CGSN=1");
        this->connState = STATE_WAITING_IMEI;
      }
    }

    if (this->connState == STATE_FINISH_IMEI) {
      if (timerCSQ.autoExpired(1000UL)) {
        NBIOT_SERIAL.println("AT+CSQ");
        this->connState = STATE_WAITING_CSQ;
      }
    }

    if (this->connState == STATE_FINISH_CSQ) {
      if (timerCGATT.autoExpired(1000UL)) {
        NBIOT_SERIAL.println("AT+CGATT?");
        this->connState = STATE_WAITING_CGATT;
      }
    }

    if (this->connState == STATE_FINISH_CGATT) {
      if (timerOPEN.autoExpired(1000UL)) {
        NBIOT_SERIAL.println("AT+QMTOPEN=0,8.210.84.24,1880");
        this->connState = STATE_WAITING_OPEN;
      }
    }

    if (this->connState == STATE_FINISH_OPEN) {
      if (timerCONN.autoExpired(1000UL)) {
        NBIOT_SERIAL.println("AT+QMTCONN=0,dev_" + String(this->IMEI) + ",tswh,1Wo=[6vA0m");
        this->connState = STATE_WAITING_CONN;
      }
    }

    if (this->connState == STATE_FINISH_CONN) {
      this->connState = STATE_WAITING_PUBSUB;
      this->finishInit = true;
    }
  }

  void listen() {
    if (NBIOT_SERIAL.available() > 0) {
      while (NBIOT_SERIAL.available() > 0) {

        char _byte = NBIOT_SERIAL.read();

        // Serial.print(_byte);
        if (_byte != '\r' && _byte != '\n') {
          this->res += _byte;
        }

        if (_byte == '\r') {
          this->handleStateChange();
          this->handleReadMsg();
          this->handleFailure();
          this->clearResBuffer();
        }
        delay(1);
      }
    }
  }

  void handleStateChange() {
    int idx = -1;

    if (this->connState == STATE_WAITING_IP) {
      idx = this->res.indexOf("+IP:");
      if (idx > -1) {
        Serial.print("\r\nFINISH WAITING IP\r\n");
        this->connState = STATE_FINISH_IP;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_SETUP) {
      Serial.print("\r\nFINISH SETUP\r\n");
      this->connState = STATE_FINISH_SETUP;
      nbiot_wdt.pet();
    }

    if (this->connState == STATE_WAITING_IMEI) {
      idx = this->res.indexOf("+CGSN:");
      if (idx > -1) {
        Serial.print("\r\FINISH GETTING IMEI\r\n");
        this->connState = STATE_FINISH_IMEI;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CSQ) {
      idx = this->res.indexOf("+CSQ:");
      if (idx > -1) {
        Serial.print("\r\FINISH GETTING CSQ\r\n");
        this->connState = STATE_FINISH_CSQ;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CGATT) {
      idx = this->res.indexOf("+CGATT:");
      if (idx > -1) {
        Serial.print("\r\FINISH GETTING CGATT\r\n");
        this->connState = STATE_FINISH_CGATT;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_OPEN) {
      idx = this->res.indexOf("+QMTOPEN: 0,0");
      if (idx > -1) {
        Serial.print("\r\FINISH OPEN\r\n");
        this->connState = STATE_FINISH_OPEN;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CONN) {
      idx = this->res.indexOf("+QMTCONN: 0,0,0");
      if (idx > -1) {
        Serial.print("\r\FINISH CONN\r\n");
        this->connState = STATE_FINISH_CONN;
        nbiot_wdt.pet();
      }
    }
  }

  void handleReadMsg() {
    int idx = -1;
    idx = this->res.indexOf("+CGSN:");
    if (idx > -1) {
      this->IMEI = this->res.substring(7, 7 + 15);

      if (!utils.isNumeric(this->IMEI)) {
        softReset = true;
      }

      // Serial.print(this->IMEI);
    }

    idx = this->res.indexOf("+CGATT:");
    if (idx > -1) {
      this->CGATT = this->res.substring(8, 8 + 1);
      // Serial.print(this->CGATT);

      if (this->CGATT != "1") {
        softReset = true;
      }

      nbiot_wdt.pet();
    }

    idx = this->res.indexOf("+CSQ:");
    if (idx > -1) {
      int winStart = idx + 6;
      int winEnd = winStart + 2;
      this->CSQ = this->res.substring(winStart, winEnd);
      // Serial.print(this->CSQ);

      if (this->CSQ == "99") {
        softReset = true;
      }

      String strNum = this->CSQ;

      if (!utils.isNumeric(strNum)) {
        softReset = true;
      }

      int numCSQ = strNum.toInt();
      if (!(numCSQ >= 15 && numCSQ <= 31)) {
        softReset = true;
      }

      this->CSQ = String(numCSQ);
      nbiot_wdt.pet();
    }
  }

  void handleFailure() {
    int idx = -1;

    idx = this->res.indexOf("+QMTOPEN: 0,-1");
    if (idx > -1) {
      softReset = true;
    }

    idx = this->res.indexOf("+QNBIOTEVENT:");
    if (idx > -1) {
      softReset = true;
    }

    idx = this->res.indexOf("+CPIN: NOT READY");
    if (idx > -1) {
      softReset = true;
    }
  }

  void publish() {
    if (this->connState == STATE_WAITING_PUBSUB) {
      if (timerPUB.autoExpired(30UL * 1000UL)) {
        String content = "{\"seq\":1,\"csq\":" + this->CSQ + ",\"cgatt\":" + this->CGATT + ",\"din\":255}";
        int contentLen = content.length();
        String cmd = "AT+QMTPUB=0,0,0,0,rgt/" + String(IMEI) + "/in," + String(contentLen) + "," + content;

        Serial.println("Regular Publish");
        NBIOT_SERIAL.println(cmd);
        this->pubState = STATE_QUERY_DEFAULT;
      } else {
        if (this->pubState == STATE_QUERY_DEFAULT) {
          if (timerStatusCSQ.autoExpired(10UL * 1000UL)) {
            this->pubState = STATE_QUERY_STAT_CSQ;
            NBIOT_SERIAL.println("AT+CSQ");
          }
        }
        if (this->pubState == STATE_QUERY_STAT_CSQ) {
          if (timerStatusCGATT.autoExpired(10UL * 1000UL)) {
            this->pubState = STATE_QUERY_STAT_CGATT;
            NBIOT_SERIAL.println("AT+CGATT?");
          }
        }
      }
    }
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif