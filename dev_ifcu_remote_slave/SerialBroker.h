#include "Globals.h"
#include "AutoTimer.h"

#ifndef SerialBroker_H
#define SerialBroker_H

AutoTimer timer;

class SerialBroker {
private:
  bool isReceiving = false;
  bool payloadReady = false;
  uint8_t bufferIdx = 0;

  uint8_t serialRecvBuffer[14] = { 0x5B,
                                   1,
                                   1,
                                   1,
                                   (2500 & 0xFF),
                                   ((2500 >> 8) & 0xFF),
                                   (2500 & 0xFF),
                                   ((2500 >> 8) & 0xFF),
                                   (2500 & 0xFF),
                                   ((2500 >> 8) & 0xFF),
                                   (2500 & 0xFF),
                                   ((2500 >> 8) & 0xFF),
                                   0,
                                   0x5D };

  // uint8_t serialRecvBuffer[8] = { 0x5B, 1, (2500 & 0xFF), ((2500 >> 8) & 0xFF), 2, 2, 210, 0x5D };
  uint8_t serialSendBuffer[8] = {};

  void clearSerialBuffer() {
    while (Serial2.read() > 0) { delay(1); };
  }

  void printlnFlush(String cmd, unsigned int delayTime = 2) {
    Serial2.println(cmd);
    Serial2.flush();
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
    if (timer.autoExpire(1000)) {
      this->handleSendBuffer();
      this->handleRecvBuffer();
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
      uint8_t payloadChecksum = this->serialRecvBuffer[12];
      uint8_t calculatedChecksum = this->getChecksum(this->serialRecvBuffer, 1, 13);
      if (payloadChecksum == calculatedChecksum) {
        responseValues[0] = this->serialRecvBuffer[1];
        responseValues[1] = this->serialRecvBuffer[2];
        responseValues[2] = this->serialRecvBuffer[3];
        responseValues[3] = (this->serialRecvBuffer[5] << 8) | this->serialRecvBuffer[4];
        responseValues[4] = (this->serialRecvBuffer[7] << 8) | this->serialRecvBuffer[6];
        responseValues[5] = (this->serialRecvBuffer[9] << 8) | this->serialRecvBuffer[8];
        responseValues[6] = (this->serialRecvBuffer[11] << 8) | this->serialRecvBuffer[10];
      }
    }
  }

  void handleSendBuffer() {
    requestValues[0] = 1;
    requestValues[1] = 2500;
    requestValues[2] = 1;
    requestValues[3] = 1;

    this->serialSendBuffer[0] = 0x5B;
    this->serialSendBuffer[1] = requestValues[0];
    this->serialSendBuffer[2] = requestValues[1] & 0xFF;
    this->serialSendBuffer[3] = (requestValues[2] >> 8) & 0xFF;
    this->serialSendBuffer[4] = requestValues[2];
    this->serialSendBuffer[5] = requestValues[3];
    this->serialSendBuffer[6] = this->getChecksum(this->serialSendBuffer, 1, 5);
    this->serialSendBuffer[7] = 0x5D;

    Serial2.write(this->serialSendBuffer, sizeof(this->serialSendBuffer));
  }

  ~SerialBroker(){};
};

#endif