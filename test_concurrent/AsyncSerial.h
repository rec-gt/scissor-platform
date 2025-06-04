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

    // if (idx != -1) {
    //   int winStart = idx + 6;
    //   int winEnd = winStart + 2;
    //   Serial.println(this->res.substring(winStart, winEnd));
    // }
  }

  void parseMsg() {
    // readonly, never modify msg
    this->hookCSQ();
    this->hookCEREG();
    if (this->res.indexOf("AT+GATT") != -1) {
    } else if (this->res.indexOf("AT+CEREG") != -1) {
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