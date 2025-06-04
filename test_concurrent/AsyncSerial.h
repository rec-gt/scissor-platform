#ifndef AsyncSerial_h
#define AsyncSerial_h

#define NBIoT_Module Serial1

class AsyncSerial {
private:
  String res = "";
  bool tryOpen = false;
  bool tryConn = false;
  bool trySubs = false;
  bool tryOpenErrCnt = 0;
  bool tryConnErrCnt = 0;
  bool trySubsErrCnt = 0;

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
    int idx = this->res.indexOf("+QMTOPEN: 0,0");
    if (idx != -1) {
      // reset open trial
      this->tryOpen = false;
      this->tryOpenErrCnt = 0;

      // goto connection phase
      this->tryConn = true;
    }
  }

  void hookTryConn() {
    int idx = this->res.indexOf("+QMTCONN: 0,0,0");
    if (idx != -1) {
      // reset conn trial
      this->tryConn = false;
      this->tryConnErrCnt = 0;

      this->trySubs = true;
    }
  }

  void hookTrySubs() {
    int idx = this->res.indexOf("+QMTSUB: 0,1,0,2");
    if (idx != -1) {
      // reset conn trial
      this->trySubs = false;
      this->trySubsErrCnt = 0;
    }
  }

  void parseMsg() {
    // readonly, never modify msg
    this->hookCSQ();
    this->hookCEREG();

    if (tryOpen) {
      this->hookTryOpen();
    }

    if (tryConn) {
      this->hookTryConn();
    }
  }

public:
  AsyncSerial() {}

  void waitMsg() {
    if (NBIoT_Module.available() > 0) {
      char _byte = NBIoT_Module.read();

      if (_byte != '\r' && _byte != '\n') {
        this->res += _byte;
      }

      if (_byte == '\r') {
        parseMsg();
        pruneResBuffer();
      }
    }
  }

  ~AsyncSerial() {}
};


extern AsyncSerial asyncSerial;

#endif