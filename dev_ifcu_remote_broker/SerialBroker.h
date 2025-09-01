#include "Globals.h"

#ifndef SerialBroker_H
#define SerialBroker_H

class SerialBroker {
private:
  bool isReceiving = false;
  uint16_t idx = 0;

  uint8_t serialRecvBuffer[8] = { 0x5B, 1, (2500 & 0xFF), ((2500 >> 8) & 0xFF), 2, 2, 210, 0x5D };

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

  uint8_t getChecksum(uint8_t *buffer, uint8_t idx_from, uint8_t idx_to) {
    uint8_t checksum = 0;
    for (uint8_t i = idx_from; i <= idx_to; i++) {
      Serial.print(buffer[i]);
      Serial.print("->");
      checksum += buffer[i];
    }
    return checksum;
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

  void captureRecvPayload() {

    requestValues[0] = this->serialRecvBuffer[1];
    requestValues[1] = (this->serialRecvBuffer[2] << 8) | this->serialRecvBuffer[3];
    requestValues[2] = this->serialRecvBuffer[4];
    requestValues[3] = this->serialRecvBuffer[5];

    // int idx = this->serialRecv.indexOf("PAYLOAD:");
    // if (idx > -1) {
    //   this->serialRecvPayload = this->serialRecv.substring(8, 100);

    //   this->strToArr(this->serialRecvPayload, requestValues, REQUEST_VALUES_LEN);

    //   Serial.print("\r\n===================\r\n");
    //   Serial.println(this->serialRecv);
    //   Serial.println(this->serialRecvPayload);

    //   for (int i = 0; i < 4; i++) {
    //     Serial.print(requestValues[i]);
    //     Serial.print(", ");
    //   }
    // }
  }


public:
  String serialRecvPayload = "";
  String serialSendPayload = "";



  SerialBroker() {
    Serial.print("\r\n===================\r\n");
    Serial.print(this->getChecksum(this->serialRecvBuffer, 1, 5));
  };

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

  void listenByte() {
    while (Serial1.available() > 0) {
      uint8_t rb = Serial1.read();

      if (rb == 0x5B) {
        this->isReceiving = true;
        this->idx = 0;
      }

      if (this->isReceiving) {
        this->serialRecvBuffer[idx++] = rb;

        if (rb == 0x5D) {
          this->isReceiving = false;
          this->idx = 0;

          uint8_t payloadChecksum = this->serialRecvBuffer[6];
          uint8_t calculatedChecksum = this->getChecksum(this->serialRecvBuffer, 1, 5);
          if (payloadChecksum == calculatedChecksum) {
            this->captureRecvPayload();
          }
        }
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