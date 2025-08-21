#include "AsyncTimer.h"
#include "Utils.h"
#include "Watchdog.h"
#include "Globals.h"
#include "Queue.h"

#ifndef WebClient_h
#define WebClient_h

#define WebClientSerial Serial1

class WebClient {
private:
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

  String readCmd() {
    if (recvCmd.length() > 0) {
      String tmp = recvCmd;
      recvCmd = "";
      return tmp;
    }

    return "";
  }

  void sendBuffer() {
    const uint8_t START_MARKER = 0xFF;

    for (uint16_t i = 0; i < sizeof(webClientSendBytes); i++) {
      Serial.print(webClientSendBytes[i]);
      Serial.print(", ");
    }
    Serial.println();

    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 7; i++) {
      checksum += webClientSendBytes[i];
    }

    WebClientSerial.write(START_MARKER);
    WebClientSerial.write(webClientSendBytes, sizeof(webClientSendBytes));
    WebClientSerial.write(checksum);
  }

  void recvBuffer() {
    while (WebClientSerial.available() > 0) {
      char c = WebClientSerial.read();
      Serial.print(c);

      if (c != '\r' && c != '\n') {
        serialRes += c;
      }

      if (c == '\r') {
        int idx = -1;

        idx = serialRes.indexOf("CMD:0");
        if (idx > -1) {
          queue.push(0);
        }

        idx = serialRes.indexOf("CMD:1");
        if (idx > -1) {
          queue.push(1);
        }

        idx = serialRes.indexOf("CMD:11");
        if (idx > -1) {
          queue.push(11);
        }

        idx = serialRes.indexOf("CMD:12");
        if (idx > -1) {
          queue.push(12);
        }

        this->clearResBuffer();
      }
    }
  }

  ~WebClient() {}
};

extern WebClient webClient;

#endif