#include "Globals.h"

#ifndef SerialBroker_H
#define SerialBroker_H

class SerialBroker {
private:
  String serialRecv = "PAYLOAD:1,2500,2,2";
  String serialSend = "PAYLOAD:1,2500,2500,2,2,1700,3000";

  void clearSerialBuffer() {
    while (Serial1.read() > 0) { delay(1); };
  }

  void clearSerialRecv() {
    this->serialRecv = "";
  }

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
    Serial1.println(cmd);
    Serial1.flush();
    delay(delayTime);
  }

  void captureRecvPayload() {
    int idx = this->serialRecv.indexOf("PAYLOAD:");
    if (idx > -1) {
      this->serialRecvPayload = this->serialRecv.substring(8, 100);

      this->strToArr(this->serialRecvPayload, requestValues, REQUEST_VALUES_LEN);

      Serial.println("===================");

      for (int i = 0; i < 4; i++) {
        Serial.print(requestValues[i]);
        Serial.print(", ");
      }
    }
  }

  void strToArr(String input, int *target, int size) {
    int index = 0;
    String temp = "";

    for (int i = 0; i < input.length(); i++) {
      char c = input[i];

      if (c == ",") {
        target[index++] = temp.toInt();
        temp = "";
      } else {
        temp += c;
      }
    }

    target[index] = temp.toInt();
  }

public:
  String serialRecvPayload = "";
  String serialSendPayload = "";



  SerialBroker(){};

  void listen() {
    this->captureRecvPayload();
    delay(1000);

    while (Serial1.available() > 0) {
      char c = Serial1.read();

      Serial.print(c);

      if (c != '\r' && c != '\n') {
        this->serialRecv += c;
      }

      if (c == '\r') {
        this->captureRecvPayload();
        this->clearSerialRecv();
      }
    }
  }

  void setRequestValues(int *values) {
    for (size_t i = 0; i < 4; i++) {
      values[i] = 0;
    }
  }

  void setResponseValues(int *values) {
    for (size_t i = 0; i < 7; i++) {
      values[i] = 0;
    }
  }

  ~SerialBroker(){};
};

#endif