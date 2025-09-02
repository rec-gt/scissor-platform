#include "Globals.h"
#include "AutoTimer.h"

#ifndef SerialBroker_H
#define SerialBroker_H

AutoTimer serialTimer;

class SerialBroker {
private:
  bool isReceiving = false;
  bool payloadReady = false;
  uint8_t bufferIdx = 0;

  uint8_t serialRecvBuffer[8] = {};
  uint8_t serialSendBuffer[14] = {};

  void clearSerialBuffer() {
    while (Serial1.read() > 0) { delay(1); };
  }

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
    Serial1.println(cmd);
    Serial1.flush();
    delay(delayTime);
  }

  uint8_t getChecksum(uint8_t *buffer, uint8_t idx_from, uint8_t idx_to) {
    uint8_t checksum = 0;
    for (uint8_t i = idx_from; i <= idx_to; i++) {
      checksum += buffer[i];
    }
    return checksum;
  }


public:
  SerialBroker(){};

  void loop() {
    this->listenByte();
    this->handleRecvBuffer();

    if (serialTimer.autoExpire(1000)) {
      this->handleSendBuffer();
    }
  }

  void listenByte() {
    while (Serial2.available() > 0) {
      this->payloadReady = false;

      uint8_t rb = Serial2.read();

      if (rb == 0x5B) {
        this->isReceiving = true;
        this->bufferIdx = 0;
      }

      if (this->isReceiving) {
        this->serialRecvBuffer[bufferIdx++] = rb;

        if (rb == 0x5D) {
          this->isReceiving = false;
          this->payloadReady = true;
          this->bufferIdx = 0;
        }
      }
    }
  }

  void handleRecvBuffer() {
    if (this->payloadReady) {
      uint8_t payloadChecksum = this->serialRecvBuffer[6];
      uint8_t calculatedChecksum = this->getChecksum(this->serialRecvBuffer, 1, 5);
      if (payloadChecksum == calculatedChecksum) {
        requestValues[0] = this->serialRecvBuffer[1];
        requestValues[1] = (this->serialRecvBuffer[3] << 8) | this->serialRecvBuffer[2];
        requestValues[2] = this->serialRecvBuffer[4];
        requestValues[3] = this->serialRecvBuffer[5];
      }
    }
  }

  void handleSendBuffer() {
    this->serialSendBuffer[0] = 0x5B;
    this->serialSendBuffer[1] = responseValues[0];
    this->serialSendBuffer[2] = responseValues[1];
    this->serialSendBuffer[3] = responseValues[2];
    this->serialSendBuffer[4] = responseValues[3] & 0xFF;
    this->serialSendBuffer[5] = (responseValues[3] >> 8) & 0xFF;
    this->serialSendBuffer[6] = responseValues[4] & 0xFF;
    this->serialSendBuffer[7] = (responseValues[4] >> 8) & 0xFF;
    this->serialSendBuffer[8] = responseValues[5] & 0xFF;
    this->serialSendBuffer[9] = (responseValues[5] >> 8) & 0xFF;
    this->serialSendBuffer[10] = responseValues[6] & 0xFF;
    this->serialSendBuffer[11] = (responseValues[6] >> 8) & 0xFF;
    this->serialSendBuffer[12] = this->getChecksum(this->serialSendBuffer, 1, 11);
    this->serialSendBuffer[13] = 0x5D;
    Serial.println("send");
    Serial.println( (this->serialSendBuffer[5] << 8) | this->serialSendBuffer[4]);
    printResponseValues();
    Serial1.write(this->serialSendBuffer, sizeof(this->serialSendBuffer));
  }

  ~SerialBroker(){};
};

#endif