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

  uint8_t serialRecvBuffer[8] = {};
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

  void captureRecvPayload() {
    requestValues[0] = this->serialRecvBuffer[1];
    requestValues[1] = (this->serialRecvBuffer[3] << 8) | this->serialRecvBuffer[2];
    requestValues[2] = this->serialRecvBuffer[4];
    requestValues[3] = this->serialRecvBuffer[5];
  }


public:
  SerialBroker(){};

  void loop() {
    this->listenByte();
    if (timer.autoExpire(1000)) {
      this->handleSendBuffer();
    }
  }

  void listenByte() {
    // while (Serial2.available() > 0) {
    //   this->payloadReady = false;

    //   uint8_t rb = Serial2.read();

    //   if (rb == 0x5B) {
    //     this->isReceiving = true;
    //     this->bufferIdx = 0;
    //   }

    //   if (this->isReceiving) {
    //     this->serialRecvBuffer[bufferIdx++] = rb;

    //     if (rb == 0x5D) {
    //       this->isReceiving = false;
    //       this->payloadReady = true;
    //       this->bufferIdx = 0;
    //     }
    //   }
    // }
  }

  void handleRecvBuffer() {
    if (this->payloadReady || true) {
      uint8_t payloadChecksum = this->serialRecvBuffer[6];
      uint8_t calculatedChecksum = this->getChecksum(this->serialRecvBuffer, 1, 5);
      if (payloadChecksum == calculatedChecksum) {
        this->captureRecvPayload();
      }
    }
  }

  void handleSendBuffer() {
    requestValues[0] = 1;
    requestValues[1] = 2500;
    requestValues[2] = 1;
    requestValues[3] = 1;
    this->serialSendBuffer = { 0x5B, requestValues[0], (requestValues[1] & 0xFF), ((requestValues[2] >> 8) & 0xFF), requestValues[2], requestValues[3], 0, 0x5D };
    this->serialSendBuffer[6] = getChecksum(this->serialSendBuffer, 1, 5);

    Serial2.write(this->serialSendBuffer, sizeof(this->serialSendBuffer));
  }

  ~SerialBroker(){};
};

#endif