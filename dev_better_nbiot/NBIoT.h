#include "AsyncTimer.h"
#include "Enums.h"
#include "Watchdog.h"
#include "Utils.h"

#ifndef NBIoT_h
#define NBIoT_h

#define NBIOT_SERIAL Serial1

#define NBIOT_INIT 0
#define NBIOT_CAN_START 1
#define NBIOT_CAN_OPEN 2
#define NBIOT_CAN_CONN 3
#define NBIOT_CAN_PUB 4
#define NBIOT_CAN_SUB 5

AsyncTimer timerNBIoT(5000);

class NBIoT {
private:
  byte connState = NBIOT_INIT;

  String res = "";

  unsigned long waitDataMillis = 0;

  String CSQ = "10";
  String IMEI = "861096060571706";

  byte resetPin = 30;

  void clearSerialBuffer() {
    while (NBIOT_SERIAL.read() > 0) {};
  }

  void clearResBuffer() {
    this->res = "";
  }

public:
  bool initFlag = false;

  NBIoT() {
    pinMode(this->resetPin, OUTPUT);
    digitalWrite(this->resetPin, HIGH);
  }

  void init() {
    this->start();
    this->clearSerialBuffer();
    this->clearResBuffer();
  }

  void start() {
    if (this->connState == NBIOT_INIT) {
      Serial.print("\r\nNBIoT RESET\r\n");
      NBIOT_SERIAL.println("AT+QRST=1");
      delay(10);
      this->connState = NBIOT_CAN_START;
    } else {
      this->connState == NBIOT_INIT;
    }
  }

  void listen() {
    if (NBIOT_SERIAL.available() > 0) {
      char _byte = NBIOT_SERIAL.read();

      Serial.print(_byte);
      if (_byte != '\r' && _byte != '\n') {
        this->res += _byte;
      }

      if (_byte == '\r') {
        this->handleStateChange();
        this->handleReadMsg();
        this->clearResBuffer();
      }
    }

    this->handleInteract();  // must put outside
  }

  void handleStateChange() {
    int idx = -1;

    // ====================================
    if (this->connState == NBIOT_CAN_START) {
      idx = this->res.indexOf("+IP:");
      if (idx > -1) {
        NBIOT_SERIAL.println("AT+CFUN=1");
        NBIOT_SERIAL.println("AT+QSCLK=0");
        NBIOT_SERIAL.println("AT+CPSMS=0");
        NBIOT_SERIAL.println("AT+CSCON=0");
        NBIOT_SERIAL.println("AT+CEDRXS=0,5");
        NBIOT_SERIAL.println("AT+QMTCLOSE=0");
        NBIOT_SERIAL.println("AT+QMTDISC=0");
        NBIOT_SERIAL.println("AT+QMTOPEN=0,8.210.84.24,1880");

        Serial.print("\r\nOpening MQTT...\r\n");

        this->connState = NBIOT_CAN_OPEN;
      }
    }

    // ====================================

    if (this->connState == NBIOT_CAN_OPEN) {
      idx = this->res.indexOf("+QMTOPEN: 0,0");
      if (idx != -1) {
        NBIOT_SERIAL.println("AT+QMTCONN=0,dev" + String(IMEI) + ",tswh,1Wo=[6vA0m");
        this->connState = NBIOT_CAN_CONN;
      }
    }

    // ====================================

    if (this->connState == NBIOT_CAN_CONN) {
      idx = this->res.indexOf("+QMTCONN: 0,0,0");
      if (idx != -1) {
        this->connState = NBIOT_CAN_PUB;
        Serial.print("\r\nCONNECTED, CAN PUBLISH\r\n");
      }
    }

    // ==================FAILURE HANDLING==================

    if (this->connState == NBIOT_CAN_CONN || this->connState == NBIOT_CAN_PUB || this->connState == NBIOT_CAN_SUB) {
      idx = this->res.indexOf("ERROR");
      if (idx != -1) {
        this->connState = NBIOT_INIT;
      }
    }
  }

  void handleReadMsg() {
    int idx = -1;

    idx = this->res.indexOf("+CSQ:");
    if (idx != -1) {
      int winStart = idx + 6;
      int winEnd = winStart + 2;
      this->CSQ = this->res.substring(winStart, winEnd);

      if (this->connState == NBIOT_CAN_PUB || this->connState == NBIOT_CAN_SUB) {
        if (this->CSQ == "99") {
          this->connState = NBIOT_INIT;
        }

        String strNum = this->CSQ;

        if (!isNumber(strNum)) {
          this->connState = NBIOT_INIT;
        }

        int num = strNum.toInt();
        if (!(num >= 15 && num <= 31)) {
          this->connState = NBIOT_INIT;
        }
      }
    }
  }

  void publish() {
    if (this->connState == NBIOT_CAN_PUB) {
      String content = "{\"seq\":1,\"csq\":" + this->CSQ + ",\"din\":255}";
      int contentLen = content.length();
      String cmd = "AT+QMTPUB=0,0,0,0,rgt/" + String(IMEI) + "/in," + String(contentLen) + "," + content;
      Serial.println("Regular Publish");
      NBIOT_SERIAL.println(cmd);
    }
  }

  void handleInteract() {
    if (timerNBIoT.isExpired()) {
      if (this->connState == NBIOT_CAN_PUB || this->connState == NBIOT_CAN_SUB || this->connState == NBIOT_CAN_CONN) {
        NBIOT_SERIAL.println("AT+CSQ");
      }
      timerNBIoT.refresh();
    }
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif