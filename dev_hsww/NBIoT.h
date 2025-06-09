#ifndef NBIoT_h
#define NBIoT_h

#define NBIoT_Serial Serial1

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
  unsigned long tryStartMillis = 0;
  unsigned long tryOpenMillis = 0;
  unsigned long tryConnMillis = 0;
  unsigned long trySubsMillis = 0;
  unsigned long waitDataMillis = 0;
  int tryResetCnt = 0;
  int tryStartCnt = 0;
  int tryOpenCnt = 0;
  int tryConnCnt = 0;
  int trySubsCnt = 0;

  void print() {
    Serial.println("isOpen: " + String(isOpen) + " isConn: " + String(isConn) + " isSubs: " + String(isSubs) + " tryOpen: " + String(tryOpen) + " tryConn: " + String(tryConn) + " trySubs: " + String(trySubs));
  }

  void pruneSerialBuffer() {
    while (NBIoT_Serial.read() > 0) {};
  }

  void pruneResBuffer() {
    this->res = "";
  }

  void hookCSQ() {
    int idx = this->res.indexOf("+CSQ:");
    if (idx != -1) {
      int winStart = idx + 6;
      int winEnd = winStart + 2;
      Serial.println(this->res.substring(winStart, winEnd));
    }
  }

  void hookCEREG() {
    int idx = this->res.indexOf("+CEREG:");
  }

  void hookTryStart() {
    if (millis() - this->tryStartMillis <= 5000) {
      int idx = this->res.indexOf("+IP:");
      if (idx != -1) {
        this->isStart = true;
        this->tryStart = false;
      }
    } else {
      this->tryStartMillis = millis();
    }
  }

  void hookTryOpen() {
    if (millis() - this->tryOpenMillis <= 5000) {
      int idx = this->res.indexOf("+QMTOPEN: 0,0");
      if (idx != -1) {
        this->isOpen = true;
        this->tryOpen = false;
      }
    } else {
      if (this->tryOpenCnt == 1) {
        NBIoT_Serial.println("AT+QMTCLOSE=0");
        NBIoT_Serial.println("AT+QMTOPEN=0,8.210.84.24,1880");
      }
      if (++this->tryOpenCnt > 3) {
        this->isOpen = false;
        this->tryOpen = false;
        this->tryOpenCnt = 0;
      }
      if (++this->tryResetCnt > 6) {
        NBIoT_Serial.println("AT+QRST=1");
        delay(5000);
        NBIoT_Serial.println("AT+CFUN=1");
        delay(100);
        NBIoT_Serial.println("AT+QSCLK=0");
        delay(100);
        this->tryResetCnt = 0;
      }
      this->tryOpenMillis = millis();
    }
  }

  void hookTryConn() {
    if (millis() - this->tryConnMillis <= 5000) {
      int idx = this->res.indexOf("+QMTCONN: 0,0,0");
      if (idx != -1) {
        this->isConn = true;
        this->tryConn = false;
      }
    } else {
      if (this->tryConnCnt == 1) {
        NBIoT_Serial.println("AT+QMTDISC=0");
        NBIoT_Serial.println("AT+QMTCONN=0,dev" + String(random(101)) + ",tswh,1Wo=[6vA0m");
      }
      if (++this->tryConnCnt > 3) {
        this->isConn = false;
        this->tryConn = false;
        this->tryConnCnt = 0;
      }
      this->tryConnMillis = millis();
    }
  }

  void hookTrySubs() {
    if (millis() - this->trySubsMillis <= 5000) {
      int idx = this->res.indexOf("+QMTSUB: 0,1,0,2");
      if (idx != -1) {
        this->isSubs = true;
        this->trySubs = false;
      }
    } else {
      if (++this->trySubsCnt > 3) {
        this->isSubs = false;
        this->trySubs = false;
        this->trySubsCnt = 0;
      }
      this->trySubsMillis = millis();
    }
  }

  void parseMsg() {
    // readonly, never modify msg
    this->hookCSQ();
    this->hookCEREG();
  }

public:
  NBIoT() {}

  void init() {
    this->pruneSerialBuffer();
    this->pruneResBuffer();

    Serial.print("INIT ");
    NBIoT_Serial.println("AT+QRST=1");
    delay(5000);
    NBIoT_Serial.println("AT+CFUN=1");
    delay(100);
    NBIoT_Serial.println("AT+QSCLK=0");
    delay(100);
    this->pruneSerialBuffer();
    Serial.println("OK");
  }

  void listen() {
    this->start();

    if (this->isStart) {
      this->open();
      this->conn();
      this->subs();
      this->waitData();
    }

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

    this->waitDataMillis = millis();
  }

  void start() {
    if (!this->isStart && !this->tryStart) {
      this->pruneSerialBuffer();
      this->pruneResBuffer();

      Serial.println("Start NBIOT...");

      NBIoT_Serial.println("AT+QRST=1");
      NBIoT_Serial.println("AT+QSCLK=0");
      NBIoT_Serial.println("AT+CFUN=1");

      this->tryStart = true;
    }
  }

  void open() {
    if (!this->isOpen && !this->tryOpen) {
      Serial.println("Opening MQTT...");
      NBIoT_Serial.println("AT+QMTOPEN=0,8.210.84.24,1880");
      this->tryOpen = true;
    }
  }

  void conn() {
    if (this->isOpen && (!this->isConn && !this->tryConn)) {
      Serial.println("Connecting MQTT...");
      NBIoT_Serial.println("AT+QMTCONN=0,dev" + String(random(101)) + ",tswh,1Wo=[6vA0m");
      this->tryConn = true;
    }
  }

  void subs() {
    if (this->isOpen && this->isConn && (!this->isSubs && !this->trySubs)) {
      Serial.println("Subscribing Topic...");
      NBIoT_Serial.println("AT+QMTSUB=0,1,rgt/861096060571706/in,2");
      this->trySubs = true;
    }
  }

  void waitData() {
    // if received, feed this->waitDataMillis
    if (millis() - this->waitDataMillis > 15000) {
      this->isOpen = false;
      this->isConn = false;
      this->isSubs = false;
      this->tryOpen = false;
      this->tryConn = false;
      this->trySubs = false;
      this->tryOpenCnt = 0;
      this->tryConnCnt = 0;
      this->trySubsCnt = 0;
      this->waitDataMillis = millis();
      Serial.println("No data received, timeout, reconnect");
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
        this->parseMsg();
        this->pruneResBuffer();
      }
    }
  }

  ~NBIoT() {}
};

extern NBIoT nbiot;

#endif