#include "AsyncTimer.h"
#include "Enums.h"
#include "AlarmSystem.h"
#include "Watchdog.h"

#ifndef NBIoT_h
#define NBIoT_h

#define NBIoT_Serial Serial1
#define TOPIC "rgt/HSWW_TOPIC/out"

AsyncTimer timer(5000);

class NBIoT {
private:
  String res = "";

  bool isStart = false;
  bool isOpen = false;
  bool isConn = false;
  bool isSubs = false;
  bool tryStart = false;
  bool tryOpen = false;
  bool tryConn = false;
  bool trySubs = false;
  int tryResetCnt = 0;
  int tryStartCnt = 0;
  int tryOpenCnt = 0;
  int tryConnCnt = 0;
  int trySubsCnt = 0;
  unsigned long waitDataMillis = 0;

  String IMEI = "";

  byte resetPin = 30;

  void print() {
    Serial.println("isOpen: " + String(isOpen) + " isConn: " + String(isConn) + " isSubs: " + String(isSubs) + " tryOpen: " + String(tryOpen) + " tryConn: " + String(tryConn) + " trySubs: " + String(trySubs));
  }

  void pruneSerialBuffer() {
    while (NBIoT_Serial.read() > 0) {};
  }

  void pruneResBuffer() {
    this->res = "";
  }

  void parseMsg() {
    // readonly, never modify msg
    this->hookGetCSQ();
    this->hookGetIMEI();
    this->hookGetCEREG();
    this->hookGetMsg();
    this->hookMonitor();
  }

  // ==========

  void hookGetCSQ() {
    int idx = this->res.indexOf("+CSQ:");
    if (idx != -1) {
      int winStart = idx + 6;
      int winEnd = winStart + 2;
      Serial.println(this->res.substring(winStart, winEnd));
    }
  }

  void hookGetIMEI() {
    int idx = this->res.indexOf("+CGSN:");
    if (idx != -1) {
      this->IMEI = this->res.substring(7, 7 + 15);
    }
  }

  void hookGetCEREG() {
    int idx = this->res.indexOf("+CEREG:");
    if (idx > -1) {
      Serial.println(this->res);
    }
  }

  void hookGetMsg() {
    int idx = this->res.indexOf("DO:");

    if (idx > -1) {
      Serial.println();
      String payload = this->res.substring(idx + 3, idx + 3 + 8);
      alarmSystem.set(payload.toInt());

      watchdog.feed();
    }

    int idx2 = this->res.indexOf("DFO:");

    if (idx2 > -1) {
      Serial.println();
      String payload = this->res.substring(idx2 + 4, idx2 + 4 + 8);
      alarmSystem.set(payload.toInt());
      alarmSystem.update();
      alarmSystem.forcePlay();

      watchdog.feed();
    }
  }

  void hookMonitor() {
    if (this->isOpen || this->isConn || this->isSubs) {
      int idx = this->res.indexOf("RDY");
      if (idx != -1) {
        this->reset();
      }
    }
  }

  // ==========

  void listenTryHooks() {
    if (this->tryStart) {
      this->hookTryStart();
    }

    if (this->tryOpen) {
      this->hookTryOpen();
    }

    if (this->tryConn) {
      this->hookTryConn();
    }

    if (this->trySubs) {
      this->hookTrySubs();
    }
  }

  void hookTryStart() {
    if (!timer.isExpired()) {
      int idx = this->res.indexOf("+IP:");
      if (idx != -1) {
        delay(100);
        NBIoT_Serial.println("AT+CFUN=1");
        delay(100);
        NBIoT_Serial.println("AT+QSCLK=0");
        delay(100);
        NBIoT_Serial.println("AT+CPSMS=0");
        delay(100);
        NBIoT_Serial.println("AT+CSCON=0");
        delay(100);
        NBIoT_Serial.println("AT+CEDRXS=0,5");
        delay(100);
        NBIoT_Serial.println("AT+CGSN=1");

        this->isStart = true;
        this->tryStart = false;
      }
    } else {
      // err count
      timer.refresh();
    }
  }

  void hookTryOpen() {
    if (!timer.isExpired()) {
      int idx = this->res.indexOf("+QMTOPEN: 0,0");
      if (idx != -1) {
        Serial.println(this->res);
        this->isOpen = true;
        this->tryOpen = false;
      }
    } else {
      Serial.println("Open connection timeout: " + String(this->tryOpenCnt));
      this->tryOpen = true;
      if (this->tryOpenCnt++ >= 3) {
        this->reset();
      }

      timer.refresh();
    }
  }

  void hookTryConn() {
    if (!timer.isExpired()) {
      int idx = this->res.indexOf("+QMTCONN: 0,0,0");
      if (idx != -1) {
        this->isConn = true;
        this->tryConn = false;
      }
    } else {
      if (this->tryConnCnt == 1) {
        NBIoT_Serial.println("AT+QMTDISC=0");
        NBIoT_Serial.println("AT+QMTCONN=0,dev" + this->IMEI + ",tswh,1Wo=[6vA0m");
      }
      if (++this->tryConnCnt > 3) {
        this->reset();
      }
      timer.refresh();
    }
  }

  void hookTrySubs() {
    if (!timer.isExpired()) {
      int idx = this->res.indexOf("+QMTSUB: 0,1,0,0");
      if (idx != -1) {
        this->isSubs = true;
        this->trySubs = false;
      }
    } else {
      if (++this->trySubsCnt > 3) {
        this->reset();
      }
      timer.refresh();
    }
  }

  // ==========

  void reset() {
    isStart = false;
    isOpen = false;
    isConn = false;
    isSubs = false;
    tryStart = false;
    tryOpen = false;
    tryConn = false;
    trySubs = false;
    tryResetCnt = 0;
    tryStartCnt = 0;
    tryOpenCnt = 0;
    tryConnCnt = 0;
    trySubsCnt = 0;
    waitDataMillis = 0;
    digitalWrite(this->resetPin, LOW);
    delay(50);
    digitalWrite(this->resetPin, HIGH);
    delay(10);
    this->pruneSerialBuffer();
    this->pruneResBuffer();
  }

  void start() {
    if (!this->isStart && !this->tryStart) {
      Serial.println("Start NBIOT...");
      this->reset();
      NBIoT_Serial.println("AT+QRST=1");
      this->tryStart = true;
    }
  }

  void open() {
    if (!this->isOpen && !this->tryOpen) {
      Serial.println("Opening MQTT...");
      NBIoT_Serial.println("AT+QMTCLOSE=0");
      NBIoT_Serial.println("AT+QMTDISC=0");
      NBIoT_Serial.println("AT+QMTOPEN=0,8.210.84.24,1880");
      this->tryOpen = true;
    }
  }

  void conn() {
    if (this->isOpen && (!this->isConn && !this->tryConn)) {
      Serial.println("Connecting MQTT...");
      NBIoT_Serial.println("AT+QMTCONN=0,dev" + String(IMEI) + ",tswh,1Wo=[6vA0m");
      this->tryConn = true;
    }
  }

  void subs() {
    if (this->isOpen && this->isConn && (!this->isSubs && !this->trySubs)) {
      Serial.println("Subscribing Topic...");
      // NBIoT_Serial.println("AT+QMTSUB=0,1,rgt/" + String(IMEI) + "/out,2");
      NBIoT_Serial.println("AT+QMTSUB=0,1," + String(TOPIC) + ",0");
      this->trySubs = true;
    }
  }

public:
  NBIoT() {
    pinMode(this->resetPin, OUTPUT);
    digitalWrite(this->resetPin, HIGH);
  }

  void listen() {
    this->listenTryHooks();

    this->start();

    if (this->isStart) {
      this->open();
      this->conn();
      this->subs();
    }

    this->waitDataMillis = millis();
  }

  void waitMsg() {
    if (NBIoT_Serial.available() > 0) {
      char _byte = NBIoT_Serial.read();

      Serial.print(_byte);

      if (_byte != '\r' && _byte != '\n') {
        this->res += _byte;
      }

      if (_byte == '\r') {
        this->parseMsg();
        this->pruneResBuffer();
      }
    }
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif