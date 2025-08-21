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

        mbClient.holdingRegisterWrite(31, 40000, serialRes.substring(0, 1).toInt());
        delay(10);
        mbClient.holdingRegisterWrite(31, 40004, serialRes.substring(1, 5).toInt());
        delay(10);
        mbClient.holdingRegisterWrite(31, 40002, serialRes.substring(5, 6).toInt());
        delay(10);
        mbClient.holdingRegisterWrite(31, 40003, serialRes.substring(6, 7).toInt());
        delay(10);

        Serial.print(serialRes.substring(0, 1).toInt());
        Serial.print(serialRes.substring(1, 5).toInt());
        Serial.print(serialRes.substring(5, 6).toInt());
        Serial.print(serialRes.substring(6, 7).toInt());

        this->clearResBuffer();
      }
    }
  }

  ~WebClient() {}
};

extern WebClient webClient;

#endif