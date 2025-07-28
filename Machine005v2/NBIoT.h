#include "AsyncTimer.h"
#include "Utils.h"
#include "Watchdog.h"
#include "DisplayOLED.h"

#ifndef NBIoT_h
#define NBIoT_h

#define NBIOT_SERIAL Serial1

Watchdog nbiot_wdt(30UL * 1000UL);

AsyncTimer timerRESET(5UL * 1000UL);
AsyncTimer nbiotTimer(10000UL);

bool softReset = false;

class NBIoTDisplayItem {
public:
  byte nbiotState = 0;
  byte displayState = 0;
  String msg = "";

  NBIoTDisplayItem() {
    nbiotState = 0;
    displayState = 0;
    msg = "";
  }

  NBIoTDisplayItem(byte nbiotState, byte displayState, String msg) {
    this->nbiotState = nbiotState;
    this->displayState = displayState;
    this->msg = msg;
  }

  ~NBIoTDisplayItem() {}
};

class NBIoTDisplay {
public:
  int arraySize = 0;
  NBIoTDisplayItem items[32];

  NBIoTDisplay() {}

  void add(NBIoTDisplayItem item) {
    this->items[this->arraySize] = item;
    this->arraySize++;
  }
};

NBIoTDisplay nbiotDisplay;

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
    STATE_WAITING_CEREG,
    STATE_FINISH_CEREG,
    STATE_WAITING_OPEN,
    STATE_FINISH_OPEN,
    STATE_WAITING_CONN,
    STATE_FINISH_CONN,
    STATE_WAITING_PUBSUB,
  };

  enum PUBSUB_PIPELINE {
    PIPELINE_DEFAULT,
    PIPELINE_WAITING_CSQ,
    PIPELINE_FINISH_CSQ,
    PIPELINE_WAITING_CGATT,
    PIPELINE_FINISH_CGATT,
    PIPELINE_WAITING_CEREG,
    PIPELINE_FINISH_CEREG,
    PIPELINE_WAITING_PUBLISH,
    PIPELINE_FINISH_PUBLISH,
  };

  bool finishInit = false;

  byte connState = STATE_DEFAULT;
  byte pubState = PIPELINE_DEFAULT;

  String res = "";

  byte resetPin = 11;

  String publishMsg = "";

  void clearSerialBuffer() {
    while (NBIOT_SERIAL.read() > 0) {};
  }

  void clearResBuffer() {
    this->res = "";
  }

public:
  String IP = "";
  String CSQ = "";
  String IMEI = "";
  String CGATT = "";
  String CEREG = "";

  NBIoT() {
    pinMode(this->resetPin, OUTPUT);
    digitalWrite(this->resetPin, LOW);

    nbiotDisplay.add(NBIoTDisplayItem(STATE_DEFAULT, DISPLAY_IOT_INIT, "正在加載IoT系統"));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_WAITING_IP, DISPLAY_IOT_WAITING_IP, "WAITING IP..."));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_FINISH_IP, DISPLAY_IOT_FINISH_IP, "IP"));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_WAITING_SETUP, DISPLAY_IOT_WAITING_SETUP, "SETTING UP IOT..."));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_FINISH_SETUP, DISPLAY_IOT_FINISH_SETUP, "IOT SETUP OK"));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_WAITING_IMEI, DISPLAY_IOT_WAITING_IMEI, "WAITING IMEI..."));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_FINISH_IMEI, DISPLAY_IOT_FINISH_IMEI, "IMEI"));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_WAITING_CSQ, DISPLAY_IOT_WAITING_CSQ, "WAITING CSQ..."));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_FINISH_CSQ, DISPLAY_IOT_FINISH_CSQ, "CSQ"));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_WAITING_CGATT, DISPLAY_IOT_WAITING_CGATT, "WAITING CGATT..."));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_FINISH_CGATT, DISPLAY_IOT_FINISH_CGATT, "CGATT"));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_WAITING_CEREG, DISPLAY_IOT_WAITING_CEREG, "WAITING CEREG..."));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_FINISH_CEREG, DISPLAY_IOT_FINISH_CEREG, "CEREG"));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_WAITING_OPEN, DISPLAY_IOT_WAITING_OPEN, "IOT OPENING..."));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_FINISH_OPEN, DISPLAY_IOT_FINISH_OPEN, "IOT OPEN OK"));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_WAITING_CONN, DISPLAY_IOT_WAITING_CONN, "IOT CONNECTING..."));
    nbiotDisplay.add(NBIoTDisplayItem(STATE_FINISH_CONN, DISPLAY_IOT_FINISH_CONN, "IOT CONN OK"));
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
    Serial.print("\r\n[NBIOT STARTING]\r\n");
    nbiot_wdt.setCallback([]() {
      softReset = true;
    });
    this->resetBuffer();
    this->loop();
  }

  void handleDisplay() {
    for (size_t i = 0; i < nbiotDisplay.arraySize; i++) {
      if (this->connState == nbiotDisplay.items[i].nbiotState) {
        String msgStr = nbiotDisplay.items[i].msg;
        if (this->connState == STATE_FINISH_IP) {
          msgStr += " " + this->IP;
        } else if (this->connState == STATE_FINISH_IMEI) {
          msgStr += " " + this->IMEI;
        } else if (this->connState == STATE_FINISH_CSQ) {
          msgStr += " " + this->CSQ;
        } else if (this->connState == STATE_FINISH_CGATT) {
          msgStr += " " + this->CGATT;
        } else if (this->connState == STATE_FINISH_CEREG) {
          msgStr += " " + this->CGATT;
        }

        char* msg = msgStr.c_str();

        displayOLED.print("", msg, "", nbiotDisplay.items[i].displayState);
        delay(1000);
      }
    }
  }

  void loop() {
    while (1) {
      nbiot_wdt.monitor();
      if (softReset) {
        softReset = false;
        this->connState = STATE_DEFAULT;
        Serial.print("\r\n[SOFT_RESET]\r\n");
      }

      this->ask();
      this->listen();


      if (this->finishInit) {
        break;
      } else {
        delay(1);
        this->handleDisplay();  // decouple, DO NOT execute after init
      }
    }
  }

  void ask() {
    if (this->connState == STATE_DEFAULT) {
      nbiot_wdt.enable();
      this->resetHardware();
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nSTARTING NBIOT\r\n");
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
      Serial.print("\r\nCONGIFERING NBIOT\r\n");
      NBIOT_SERIAL.println("AT+CFUN=1");
      delay(10);
      NBIOT_SERIAL.println("AT+QSCLK=0");
      delay(10);
      NBIOT_SERIAL.println("AT+CFUN=1");
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
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nGETTING IMEI\r\n");
        NBIOT_SERIAL.println("AT+CGSN=1");
        this->connState = STATE_WAITING_IMEI;
      }
    }

    if (this->connState == STATE_FINISH_IMEI) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nGETTING CSQ\r\n");
        NBIOT_SERIAL.println("AT+CSQ");
        this->connState = STATE_WAITING_CSQ;
      }
    }

    if (this->connState == STATE_FINISH_CSQ) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nGETTING CGATT\r\n");
        NBIOT_SERIAL.println("AT+CGATT?");
        this->connState = STATE_WAITING_CGATT;
      }
    }

    if (this->connState == STATE_FINISH_CGATT) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nOPENING MQTT\r\n");
        NBIOT_SERIAL.println("AT+CEREG?");
        this->connState = STATE_WAITING_CEREG;
      }
    }

    if (this->connState == STATE_FINISH_CEREG) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nOPENING MQTT\r\n");
        NBIOT_SERIAL.println("AT+QMTOPEN=0,8.210.84.24,1880");
        this->connState = STATE_WAITING_OPEN;
      }
    }

    if (this->connState == STATE_FINISH_OPEN) {
      if (nbiotTimer.autoExpired(1000UL)) {
        Serial.print("\r\nCONNECTING MQTT\r\n");
        NBIOT_SERIAL.println("AT+QMTCONN=0,dev_" + String(this->IMEI) + ",tswh,1Wo=[6vA0m");
        this->connState = STATE_WAITING_CONN;
      }
    }

    if (this->connState == STATE_FINISH_CONN) {
      this->connState = STATE_WAITING_PUBSUB;
      this->finishInit = true;
    }

    if (this->connState == STATE_WAITING_PUBSUB) {
      if (this->pubState == PIPELINE_DEFAULT) {
        if (nbiotTimer.autoExpired(3000)) {
          Serial.print("\r\nQUERYING CSQ\r\n");
          NBIOT_SERIAL.println("AT+CSQ");
          this->pubState = PIPELINE_WAITING_CSQ;
        }
      }

      if (this->pubState == PIPELINE_FINISH_CSQ) {
        if (nbiotTimer.autoExpired(3000)) {
          Serial.print("\r\nQUERYING CGATT\r\n");
          NBIOT_SERIAL.println("AT+CGATT?");
          this->pubState = PIPELINE_WAITING_CGATT;
        }
      }

      if (this->pubState == PIPELINE_FINISH_CGATT) {
        if (nbiotTimer.autoExpired(3000)) {
          Serial.print("\r\nQUERYING CEREG\r\n");
          NBIOT_SERIAL.println("AT+CEREG?");
          this->pubState = PIPELINE_WAITING_CEREG;
        }
      }

      if (this->pubState == PIPELINE_FINISH_CEREG) {
        if (nbiotTimer.autoExpired(11000)) {
          Serial.print("\r\nEXECUTE REGULAR PUBLISH\r\n");
          NBIOT_SERIAL.println(this->publishMsg);
          this->pubState = PIPELINE_WAITING_PUBLISH;
        }
      }
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
          this->answer();
          this->handleReadMsg();
          this->handleFailure();
          this->clearResBuffer();
        }
        delay(1);
      }
    }
  }

  void answer() {
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
      {
        Serial.print("\r\nFINISH SETUP\r\n");
        this->connState = STATE_FINISH_SETUP;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_IMEI) {
      idx = this->res.indexOf("+CGSN:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING IMEI\r\n");
        this->connState = STATE_FINISH_IMEI;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CSQ) {
      idx = this->res.indexOf("+CSQ:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING CSQ\r\n");
        this->connState = STATE_FINISH_CSQ;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CGATT) {
      idx = this->res.indexOf("+CGATT:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING CGATT\r\n");
        this->connState = STATE_FINISH_CGATT;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CEREG) {
      idx = this->res.indexOf("+CEREG:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING CEREG\r\n");
        this->connState = STATE_FINISH_CEREG;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_OPEN) {
      idx = this->res.indexOf("+QMTOPEN: 0,0");
      if (idx > -1) {
        Serial.print("\r\nFINISH OPENING MQTT\r\n");
        this->connState = STATE_FINISH_OPEN;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CONN) {
      idx = this->res.indexOf("+QMTCONN: 0,0,0");
      if (idx > -1) {
        Serial.print("\r\nFINISH CONNECTING MQTT\r\n");
        this->connState = STATE_FINISH_CONN;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_PUBSUB) {
      if (this->pubState == PIPELINE_WAITING_CSQ) {
        int idx = this->res.indexOf("+CSQ:");
        if (idx > -1) {
          Serial.print("\r\nFINISH GETTING CSQ\r\n");
          this->pubState = PIPELINE_FINISH_CSQ;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_CGATT) {
        int idx = this->res.indexOf("+CGATT:");
        if (idx > -1) {
          Serial.print("\r\nFINISH GETTING CGATT\r\n");
          this->pubState = PIPELINE_FINISH_CGATT;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_CEREG) {
        int idx = this->res.indexOf("+CEREG:");
        if (idx > -1) {
          Serial.print("\r\nFINISH GETTING CEREG\r\n");
          this->pubState = PIPELINE_FINISH_CEREG;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_PUBLISH) {
        int idx = this->res.indexOf("+QMTPUB:");
        if (idx > -1) {
          String QMTPUB = this->res.substring(9, 9 + 5);
          if (QMTPUB == "0,0,0") {
            nbiot_wdt.pet();
            this->pubState = PIPELINE_DEFAULT;
            Serial.print("\r\nFINISH REGULAR PUBLISH\r\n");
          }
        }
      }
    }
  }

  void handleReadMsg() {
    int idx = -1;
    idx = this->res.indexOf("+IP:");
    if (idx > -1) {
      this->IP = this->res.substring(5, 5 + 16);
      // Serial.print(this->IP);
    }

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
    }

    idx = this->res.indexOf("+CEREG:");
    if (idx > -1) {
      this->CEREG = this->res.substring(8, 8 + 3);
      // Serial.print(this->CEREG);

      if (this->CEREG != "0,1") {
        softReset = true;
      }
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
      if (!(numCSQ >= 10 && numCSQ <= 31)) {
        softReset = true;
      }

      this->CSQ = String(numCSQ);
    }

    idx = this->res.indexOf("+QMTPUB:");
    if (idx > -1) {
      String QMTPUB = this->res.substring(9, 9 + 5);
      if (QMTPUB != "0,0,0") {
        softReset = true;
      }
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

  void setPublishMsg(String msg) {
    this->publishMsg = msg;
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif