#include "Globals.h"

#ifndef SerialBroker_H
#define SerialBroker_H

class SerialBroker {
private:
  bool isReceiving = false;
  bool payloadReady = false;
  uint8_t bufferIdx = 0;

  uint8_t serialRecvBuffer[8] = {};

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

  void captureRecvPayload() {
    requestValues[0] = this->serialRecvBuffer[1];
    requestValues[1] = (this->serialRecvBuffer[3] << 8) | this->serialRecvBuffer[2];
    requestValues[2] = this->serialRecvBuffer[4];
    requestValues[3] = this->serialRecvBuffer[5];

    printRequestValues();
  }


public:
  SerialBroker(){};

  void loop() {
    this->listenByte();
    this->handleRecvBuffer();
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
        this->captureRecvPayload();
      }
    }
  }

  ~SerialBroker(){};
};

#endif