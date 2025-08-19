#include "AsyncTimer.h"
#include "Utils.h"
#include "Watchdog.h"
#include "Globals.h"

#ifndef Web_h
#define Web_h

#define WebSerial Serial1

class Web {
private:
  bool finishInit = false;

  bool debugMode = false;

  void clearSerialBuffer() {
    while (WebSerial.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    serialRes = "";
  }

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
    WebSerial.println(cmd);
    WebSerial.flush();
    delay(delayTime);
  }

public:
  Web() {
    this->clearSerialBuffer();
  }

  void listen() {
    if (WebSerial.available() > 0) {
      while (WebSerial.available() > 0) {
        char c = WebSerial.read();

        if (c != '\r' && c != '\n') {
          serialRes += c;
        }

        if (c == '\r') {
          this->readRecv();
          this->clearResBuffer();
        }
      }
    }
  }

  void readRecv() {
    int idx = serialRes.indexOf("[ASK]");

    if (idx > -1) {
      this->sendMsg();
    }
  }

  String readCmd() {
    return "-1";
  }

  void sendMsg() {
    this->printlnFlush(webSend);
    webSend = "";
  }

  ~Web() {}
};

extern Web web;

#endif