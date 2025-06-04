#ifndef AsyncSerial_h
#define AsyncSerial_h

#define NBIoT_Module Serial1

class AsyncSerial {
private:
  String res = "";

  bool isOpen = false;
  bool isConn = false;
  bool isSubs = false;
  bool tryOpen = false;
  bool tryConn = false;
  bool trySubs = false;
  int tryOpenCnt = 0;
  int tryConnCnt = 0;
  int trySubsCnt = 0;

  unsigned long tryOpenMillis = 0;
  unsigned long tryConnMillis = 0;
  unsigned long trySubsMillis = 0;

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
    Serial.println(this->res);
  }

  void hookTryOpen() {
    Serial.println(millis() - this->tryOpenMillis);
    if (millis() - this->tryOpenMillis <= 5UL * 1000UL) {
      int idx = this->res.indexOf("+QMTOPEN: 0,0");
      if (idx != -1) {
        this->isOpen = true;
        this->tryOpen = false;
      }
    } else {
      Serial.println("MQTT OPEN COUNT: " + String(this->tryOpenCnt));
      if (++this->tryOpenCnt > 3) {
        this->isOpen = false;
        this->tryOpen = false;
        this->tryOpenCnt = 0;
      }
      this->tryOpenMillis = millis();
    }
  }

  void hookTryConn() {
    if (millis() - this->tryConnMillis <= 5UL * 1000UL) {
      int idx = this->res.indexOf("+QMTCONN: 0,0,0");
      if (idx != -1) {
        this->isConn = true;
        this->tryConn = false;
      }
    } else {
      if (++this->tryConnCnt > 3) {
        this->isConn = false;
        this->tryConn = false;
        this->tryConnCnt = 0;
      }
      this->tryConnMillis = millis();
    }
  }

  void hookTrySubs() {
    if (millis() - this->trySubsMillis <= 5UL * 1000UL) {
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

public:
  AsyncSerial() {}

  void init() {
    Serial.print("INIT ");
    NBIoT_Module.println("AT+QRST=1");
    delay(10000);
    NBIoT_Module.println("AT+CFUN=1");
    delay(100);
    NBIoT_Module.println("AT+QSCLK=0");
    delay(100);
    NBIoT_Module.println("AT+QMTDISC=0");
    delay(100);
    NBIoT_Module.println("AT+QMTCLOSE=0");
    delay(100);
    NBIoT_Module.println("AT+QMTDISC=0");
    delay(100);
    NBIoT_Module.println("AT+QMTCLOSE=0");
    delay(100);
    while (NBIoT_Module.read() > 0) {};
    Serial.println("OK");
  }

  void listen() {
    this->open();
    this->conn();
    this->subs();
  }

  void open() {
    if (!this->isOpen && !this->tryOpen) {
      Serial.println("Opening MQTT...");
      NBIoT_Module.println("AT+QMTOPEN=0,8.210.84.24,1880");
      this->tryOpen = true;
    }
  }

  void conn() {
    if (this->isOpen && (!this->isConn && !this->tryConn)) {
      Serial.println("Connecting MQTT...");
      NBIoT_Module.println("AT+QMTCONN=0,dev" + String(random(101)) + ",tswh,1Wo=[6vA0m");
      this->tryConn = true;
    }
  }

  void subs() {
    if (this->isOpen && this->isConn && (!this->isSubs && !this->trySubs)) {
      Serial.println("Subscribing Topic...");
      NBIoT_Module.println("AT+QMTSUB=0,1,rgt/861096060571706/in,2");
      this->trySubs = true;
    }
  }

  void waitMsg() {
    if (NBIoT_Module.available() > 0) {
      char _byte = NBIoT_Module.read();

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

  ~AsyncSerial() {}
};


extern AsyncSerial asyncSerial;

#endif