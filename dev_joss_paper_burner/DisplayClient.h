#include "AnalogInput.h"
#include "AnalogOutput.h"

#ifndef DisplayClient_H
#define DisplayClient_H

#define DisplaySerial Serial2

class DisplayClient {
private:
  uint8_t buffer[22];

public:
  DisplayClient(){};

  void prepareBuffer(uint8_t nbiotConn, uint8_t nbiotCsq, uint8_t dis, uint8_t dos, AnalogInput *ais, AnalogOutput *aos) {
    uint8_t idx = 0;

    this->buffer[idx++] = 0x5B;  // '['
    this->buffer[idx++] = nbiotConn;
    this->buffer[idx++] = nbiotCsq;
    this->buffer[idx++] = dis;
    this->buffer[idx++] = dos;

    for (int i = 0; i < 12; i++) {
      this->buffer[idx++] = (ais[i].value >> 8) & 0xFF;
      this->buffer[idx++] = ais[i].value & 0xFF;
    }

    for (int i = 0; i < 4; i++) {
      this->buffer[idx++] = (aos[i].value >> 8) & 0xFF;
      this->buffer[idx++] = aos[i].value & 0xFF;
    }

    this->buffer[idx++] = 0x5D;  // ']'
  }

  void sendBuffer() {
    DisplaySerial.write(this->buffer, sizeof(this->buffer));
  }

  ~DisplayClient() {}
};

extern DisplayClient displayClient;

#endif