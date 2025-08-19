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
        Serial.print(c);

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

    idx = serialRes.indexOf("CMD:1");
    if (idx > -1) {
      recvCmd = "1";
    }

    idx = serialRes.indexOf("CMD:2");
    if (idx > -1) {
      recvCmd = "2";
    }

    idx = serialRes.indexOf("CMD:3");
    if (idx > -1) {
      recvCmd = "3";
    }

    idx = serialRes.indexOf("CMD:4");
    if (idx > -1) {
      recvCmd = "4";
    }

    idx = serialRes.indexOf("CMD:5");
    if (idx > -1) {
      recvCmd = "5";
    }
  }

  String readCmd() {
    if (recvCmd.length() > 0) {
      String tmp = recvCmd;
      recvCmd = "";
      return tmp;
    }

    return "";
  }

  void sendMsg() {
    Serial.println(webSend);
    this->printlnFlush(webSend);
    webSend = "";
  }

  ~Web() {}
};

extern Web web;

#endif