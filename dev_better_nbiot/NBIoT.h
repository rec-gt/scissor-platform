#include "AsyncTimer.h"
#include "Enums.h"
#include "Watchdog.h"

#ifndef NBIoT_h
#define NBIoT_h

#define NBIoT_Serial Serial1
#define TOPIC "rgt/HSWW_TOPIC/out"

#define STATE_CAN_INIT 0
#define STATE_CAN_START 1
#define STATE_CAN_OPEN 2
#define STATE_CAN_CONN 3
#define STATE_CAN_PUB 4
#define STATE_CAN_SUB 5

AsyncTimer timerNBIoT(5 * 1000);

class NBIoT {
private:
  byte connState = STATE_CAN_INIT;
  byte connTrial = 0;
  byte connCount = 0;

  String res = "";

  unsigned long waitDataMillis = 0;

  String CSQ = "10";
  String IMEI = "861096060571706";

  byte resetPin = 30;

  void clearSerialBuffer() {
    while (NBIoT_Serial.read() > 0) {};
  }

  void clearResBuffer() {
    this->res = "";
  }

public:
  NBIoT() {
    pinMode(this->resetPin, OUTPUT);
    digitalWrite(this->resetPin, HIGH);
  }

  void start() {
    if (this->connState == STATE_CAN_INIT) {
      Serial.println("NBIoT START: AT+QRST=1");
      NBIoT_Serial.println("AT+QRST=1");
      delay(100);
      this->connState = STATE_CAN_START;
    }
  }

  void waitMsg() {
    if (NBIoT_Serial.available() > 0) {
      char _byte = NBIoT_Serial.read();

      Serial.print(_byte);
      if (_byte != '\r' && _byte != '\n') {
        this->res += _byte;
      }

      if (_byte == '\r') {
        this->handleStateChange();
        this->handleReadMsg();
        this->handleInteract();
        this->clearResBuffer();
      }
    }
  }

  void handleStateChange() {
    int idx = -1;

    // ====================================
    if (this->connState == STATE_CAN_START) {
      idx = this->res.indexOf("+IP:");
      if (idx > -1) {
        NBIoT_Serial.println("AT+CFUN=1");
        delay(1);
        NBIoT_Serial.println("AT+QSCLK=0");
        delay(1);
        NBIoT_Serial.println("AT+CPSMS=0");
        delay(1);
        NBIoT_Serial.println("AT+CSCON=0");
        delay(1);
        NBIoT_Serial.println("AT+CEDRXS=0,5");
        delay(1);
        NBIoT_Serial.println("AT+QMTCLOSE=0");
        delay(1);
        NBIoT_Serial.println("AT+QMTDISC=0");
        delay(1);
        NBIoT_Serial.println("AT+QMTOPEN=0,8.210.84.24,1880");
        delay(1);

        Serial.println("Opening MQTT...");

        this->connState = STATE_CAN_OPEN;
      }
    }

    // ====================================

    if (this->connState == STATE_CAN_OPEN) {
      idx = this->res.indexOf("+QMTOPEN: 0,0");
      if (idx != -1) {
        NBIoT_Serial.println("AT+QMTCONN=0,dev" + String(IMEI) + ",tswh,1Wo=[6vA0m");
        this->connState = STATE_CAN_CONN;
      }
    }

    // ====================================

    if (this->connState == STATE_CAN_CONN) {
      idx = this->res.indexOf("+QMTCONN: 0,0,0");
      if (idx != -1) {
        this->connState = STATE_CAN_PUB;
        Serial.print("OK connected, can publish");
      }
    }

    if (this->connState == STATE_CAN_PUB) {
      idx = this->res.indexOf("ERROR");
      if (idx != -1) {
        this->connState = STATE_CAN_INIT;
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
    }
  }

  void publish() {
    if (this->connState == STATE_CAN_PUB) {
      String content = "{\"seq\":1,\"csq\":" + this->CSQ + ",\"din\":255}";
      int contentLen = content.length();
      String cmd = "AT+QMTPUB=0,0,0,0,rgt/" + String(IMEI) + "/in," + String(contentLen) + "," + content;
      Serial.println("Regular Publish");
      NBIoT_Serial.println(cmd);
    }
  }

  void handleInteract() {
    if (this->connState == STATE_CAN_PUB || this->connState == STATE_CAN_CONN || this->connState == STATE_CAN_OPEN) {
      if (timerNBIoT.isExpired()) {
        NBIoT_Serial.println("AT+CSQ");
        NBIoT_Serial.println("AT+CGSN=1");
        timerNBIoT.refresh();
      }
    }
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif