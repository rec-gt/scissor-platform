#include "AsyncTimer.h"
#include "Utils.h"
#include "Watchdog.h"
#include "DisplayOLED.h"
#include "Globals.h"

#ifndef NBIoT_h
#define NBIoT_h

#define NBIOT_SERIAL Serial1

Watchdog nbiot_wdt(30UL * 1000UL);

AsyncTimer nbiotTimer(10000UL);

bool softReset = false;

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
    PIPELINE_WAITING_PREPARE_PUBMSG,
    PIPELINE_FINISH_PREPARE_PUBMSG,
    PIPELINE_WAITING_PUBLISH,
    PIPELINE_FINISH_PUBLISH,
  };

  bool finishInit = false;

  byte prevConnState = STATE_DEFAULT;
  byte connState = STATE_WAITING_RESET;
  byte pubState = PIPELINE_DEFAULT;

  byte resetPin = 11;

  bool debugMode = false;

  void clearSerialBuffer() {
    while (NBIOT_SERIAL.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    resMsg = "";
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

  void debug() {
    this->debugMode = true;
  }

  void init() {
    Serial.print("\r\n=== NBIOT START ===\r\n");
    nbiot_wdt.enable();
    nbiot_wdt.setCallback([]() {
      softReset = true;
    });
    this->resetBuffers();
    this->loop();
  }

  void handleDisplay() {
    if (this->connState == STATE_WAITING_IP) {
      displayOLED.print("", "WAITING IP...", "", DISPLAY_IOT_WAITING_IP);
    } else if (this->connState == STATE_FINISH_IP) {
      displayOLED.print("", "IP", (this->IP).c_str(), DISPLAY_IOT_FINISH_IP);
      delay(500);
    } else if (this->connState == STATE_WAITING_IMEI) {
      displayOLED.print("", "WAITING IMEI...", "", DISPLAY_IOT_WAITING_IMEI);
    } else if (this->connState == STATE_FINISH_IMEI) {
      displayOLED.print("", "IMEI", (this->IMEI).c_str(), DISPLAY_IOT_FINISH_IMEI);
      delay(500);
    } else if (this->connState == STATE_WAITING_CSQ) {
      displayOLED.print("", "WAITING CSQ...", "", DISPLAY_IOT_WAITING_CSQ);
    } else if (this->connState == STATE_FINISH_CSQ) {
      displayOLED.print("", "CSQ", (this->CSQ).c_str(), DISPLAY_IOT_FINISH_CSQ);
      delay(500);
    } else if (this->connState == STATE_WAITING_CGATT) {
      displayOLED.print("", "WAITING CGATT...", "", DISPLAY_IOT_WAITING_CGATT);
    } else if (this->connState == STATE_FINISH_CGATT) {
      displayOLED.print("", "CGATT", (this->CGATT).c_str(), DISPLAY_IOT_FINISH_CGATT);
      delay(500);
    } else if (this->connState == STATE_WAITING_CEREG) {
      displayOLED.print("", "WAITING CEREG...", "", DISPLAY_IOT_WAITING_CEREG);
    } else if (this->connState == STATE_FINISH_CEREG) {
      displayOLED.print("", "CEREG", (this->CEREG).c_str(), DISPLAY_IOT_FINISH_CEREG);
      delay(500);
    } else if (this->connState == STATE_WAITING_OPEN) {
      displayOLED.print("", "IOT OPENING...", "", DISPLAY_IOT_WAITING_OPEN);
    } else if (this->connState == STATE_FINISH_OPEN) {
      displayOLED.print("", "FINISH IOT OPEN", "", DISPLAY_IOT_FINISH_OPEN);
      delay(500);
    } else if (this->connState == STATE_WAITING_CONN) {
      displayOLED.print("", "IOT CONNECTING...", "", DISPLAY_IOT_WAITING_CONN);
    } else if (this->connState == STATE_FINISH_CONN) {
      displayOLED.print("", "FINISH IOT CONN", "", DISPLAY_IOT_FINISH_CONN);
      delay(500);
    }
  }

  void loop() {
    while (1) {
      nbiot_wdt.monitor();

      if (softReset) {
        this->resetBuffers();
        softReset = false;
        connStr = "";
        publishMsg = "";
        publishMsgContent = "";
        this->connState = STATE_WAITING_RESET;
        this->pubState = PIPELINE_DEFAULT;
        Serial.print("\r\n[SOFT_RESET]\r\n");
      }

      this->ask();
      this->listen();

      if (this->finishInit) {
        break;
      } else {
        delay(10);
        this->handleDisplay();  // decouple, DO NOT execute after init
      }
    }
  }

  bool isAsked() {
    if (this->connState == this->prevConnState) {
      return false;
    } else {
      this->prevConnState = this->connState;
      return true;
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
        NBIOT_SERIAL.println("AT+QSCLK=0");
        delay(10);
        this->connState = STATE_WAITING_IP;
      }
    }

    if (this->connState == STATE_FINISH_IP) {
      Serial.print("\r\nSETTING UP NBIOT\r\n");
      NBIOT_SERIAL.println("AT+CFUN=1");
      delay(10);
      NBIOT_SERIAL.println("AT+QSCLK=0");
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
        Serial.print("\r\nGETTING CEREG\r\n");
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
        NBIOT_SERIAL.println(connStr);
        connStr = "";
        this->connState = STATE_WAITING_CONN;
      }
    }

    if (this->connState == STATE_FINISH_CONN) {
      this->connState = STATE_WAITING_PUBSUB;
      this->finishInit = true;
    }

    if (this->connState == STATE_WAITING_PUBSUB) {
      if (this->pubState == PIPELINE_DEFAULT) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.print("\r\nQUERYING CSQ\r\n");
          NBIOT_SERIAL.println("AT+CSQ");
          this->pubState = PIPELINE_WAITING_CSQ;
        }
      }

      if (this->pubState == PIPELINE_FINISH_CSQ) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.print("\r\nQUERYING CGATT\r\n");
          NBIOT_SERIAL.println("AT+CGATT?");
          this->pubState = PIPELINE_WAITING_CGATT;
        }
      }

      if (this->pubState == PIPELINE_FINISH_CGATT) {
        if (nbiotTimer.autoExpired(5000)) {
          Serial.print("\r\nQUERYING CEREG\r\n");
          NBIOT_SERIAL.println("AT+CEREG?");
          this->pubState = PIPELINE_WAITING_CEREG;
        }
      }

      if (this->pubState == PIPELINE_FINISH_CEREG) {
        if (nbiotTimer.autoExpired(13000)) {
          Serial.print("\r\PREPARE REGULAR PUBLISH\r\n");
          NBIOT_SERIAL.println(publishMsgPrepare);
          this->pubState = PIPELINE_WAITING_PREPARE_PUBMSG;
        }
      }

      if (this->pubState == PIPELINE_FINISH_PREPARE_PUBMSG) {
        if (nbiotTimer.autoExpired(2000)) {
          Serial.print("\r\nEXECUTE REGULAR PUBLISH\r\n");
          NBIOT_SERIAL.println(publishMsgContent);
          this->pubState = PIPELINE_WAITING_PUBLISH;
        }
      }
    }
  }

  // void listen() {
  //   if (NBIOT_SERIAL.available() > 0) {
  //     while (NBIOT_SERIAL.available() > 0) {
  //       char _byte = NBIOT_SERIAL.read();

  //       if (this->debugMode) {
  //         // Serial.print(_byte);
  //         Serial.println(resMsg);
  //       }

  //       if (_byte != '\r' && _byte != '\n') {
  //         resMsg += _byte;
  //       }

  //       if (_byte == '\r') {
  //         this->answer();
  //         this->handleReadMsg();
  //         this->clearResBuffer();
  //       }
  //       delay(1);
  //     }
  //   }
  // }

  void listen() {
    if (NBIOT_SERIAL.available() > 0) {
      while (NBIOT_SERIAL.available() > 0) {
        char _byte = NBIOT_SERIAL.read();
        delay(2);

        if (this->debugMode) {
          Serial.print(_byte);
        }

        if (_byte != '\r' && _byte != '\n') {
          resMsg += _byte;
        }

        if (_byte == '\r') {
          this->answer();
          this->handleReadMsg();
          this->clearResBuffer();
        }
      }
    }
  }

  void answer() {
    int idx = -1;

    if (this->connState == STATE_WAITING_IP) {
      idx = resMsg.indexOf("+IP:");
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
      idx = resMsg.indexOf("+CGSN:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING IMEI\r\n");
        this->connState = STATE_FINISH_IMEI;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CSQ) {
      idx = resMsg.indexOf("+CSQ:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING CSQ\r\n");
        this->connState = STATE_FINISH_CSQ;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CGATT) {
      idx = resMsg.indexOf("+CGATT:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING CGATT\r\n");
        this->connState = STATE_FINISH_CGATT;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CEREG) {
      idx = resMsg.indexOf("+CEREG:");
      if (idx > -1) {
        Serial.print("\r\nFINISH GETTING CEREG\r\n");
        this->connState = STATE_FINISH_CEREG;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_OPEN) {
      idx = resMsg.indexOf("+QMTOPEN: 0,0");
      if (idx > -1) {
        Serial.print("\r\nFINISH OPENING MQTT\r\n");
        this->connState = STATE_FINISH_OPEN;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_CONN) {
      idx = resMsg.indexOf("+QMTCONN: 0,0,0");
      if (idx > -1) {
        Serial.print("\r\nFINISH CONNECTING MQTT\r\n");
        this->connState = STATE_FINISH_CONN;
        nbiot_wdt.pet();
      }
    }

    if (this->connState == STATE_WAITING_PUBSUB) {
      int idx = -1;
      if (this->pubState == PIPELINE_WAITING_CSQ) {
        idx = resMsg.indexOf("+CSQ:");
        if (idx > -1) {
          Serial.print("\r\nFINISH GETTING CSQ\r\n");
          this->pubState = PIPELINE_FINISH_CSQ;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_CGATT) {
        idx = resMsg.indexOf("+CGATT:");
        if (idx > -1) {
          Serial.print("\r\nFINISH GETTING CGATT\r\n");
          this->pubState = PIPELINE_FINISH_CGATT;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_CEREG) {
        idx = resMsg.indexOf("+CEREG:");
        if (idx > -1) {
          Serial.print("\r\nFINISH GETTING CEREG\r\n");
          this->pubState = PIPELINE_FINISH_CEREG;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_PREPARE_PUBMSG) {
        idx = resMsg.indexOf(">");
        if (idx > -1) {
          this->pubState = PIPELINE_FINISH_PREPARE_PUBMSG;
          nbiot_wdt.pet();
        }
      }

      if (this->pubState == PIPELINE_WAITING_PUBLISH) {
        idx = resMsg.indexOf("OK");
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
    idx = resMsg.indexOf("+IP:");
    if (idx > -1) {
      this->IP = resMsg.substring(5, 5 + 16);
    }

    idx = resMsg.indexOf("+CGSN:");
    if (idx > -1) {
      this->IMEI = resMsg.substring(7, 7 + 15);

      if (!utils.isNumeric(this->IMEI)) {
        softReset = true;
      }

      connStr = "AT+QMTCONN=0,dev_";
      connStr.concat(this->IMEI);
      connStr.concat(",tswh,1Wo=[6vA0m");
    }

    idx = resMsg.indexOf("+CGATT:");
    if (idx > -1) {
      this->CGATT = resMsg.substring(8, 8 + 1);

      if (this->CGATT != "1") {
        softReset = true;
      }
    }

    idx = resMsg.indexOf("+CEREG:");
    if (idx > -1) {
      this->CEREG = resMsg.substring(8, 8 + 3);

      if (this->CEREG != "0,1") {
        softReset = true;
      }
    }

    idx = resMsg.indexOf("+CSQ:");
    if (idx > -1) {
      int winStart = idx + 6;
      int winEnd = winStart + 2;
      this->CSQ = resMsg.substring(winStart, winEnd);

      if (this->CSQ == "99") {
        softReset = true;
      }

      String strNum = this->CSQ;

      if (!utils.isNumeric(strNum)) {
        softReset = true;
      }

      int numCSQ = strNum.toInt();
      if (!(numCSQ >= 8 && numCSQ <= 31)) {
        softReset = true;
      }

      this->CSQ = String(numCSQ);
    }
  }

  void forcePublish() {
    NBIOT_SERIAL.println(publishMsg);
    NBIOT_SERIAL.flush();
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif