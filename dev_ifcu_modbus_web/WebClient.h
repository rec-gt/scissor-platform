#include "AsyncTimer.h"
#include "Utils.h"
#include "Watchdog.h"
#include "Globals.h"

#ifndef WebClient_h
#define WebClient_h

#define WebClientSerial Serial1

class WebClient {
private:
  bool finishInit = false;

  bool debugMode = false;

  void clearSerialBuffer() {
    while (WebClientSerial.read() > 0) { delay(1); };
  }

  void clearResBuffer() {
    serialRes = "";
  }

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
    WebClientSerial.println(cmd);
    WebClientSerial.flush();
    delay(delayTime);
  }

public:
  WebClient() {
    this->clearSerialBuffer();
  }

  void listen() {
    if (WebClientSerial.available() > 0) {
      while (WebClientSerial.available() > 0) {
        char c = WebClientSerial.read();
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
      this->sendBuffer();
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

  void sendBuffer() {
    for (uint16_t i = 0; i < sizeof(webClientSendBytes); i++) {
      Serial.print(webClientSendBytes[i]);
      Serial.print(", ");
    }
    Serial.println();

    const uint8_t START_MARKER = 0xFF;

    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 7; i++) {
      checksum += webClientSendBytes[i];
    }

    WebClientSerial.write(START_MARKER);
    WebClientSerial.write(webClientSendBytes, sizeof(webClientSendBytes));
    WebClientSerial.write(checksum);
  }

  ~WebClient() {}
};

extern WebClient webClient;

#endif