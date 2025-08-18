#include "AnalogInput.h"
#include "AnalogOutput.h"

#ifndef DisplayClient_H
#define DisplayClient_H

#define DisplaySerial Serial2

class DisplayClient {
private:

public:
  DisplayClient(){};

  void prepareBuffer(uint8_t nbiotConn, uint8_t csq, uint8_t dis, uint8_t dos, AnalogInput *ais, AnalogOutput *aos) {
    uint8_t buffer[41];

    buffer[0] = nbiotConn;
    buffer[1] = csq;
    buffer[2] = dis;
    buffer[3] = dos;

    int index = 4;
    for (int i = 0; i < 12; i++) {
      buffer[index++] = (ais[i] >> 8) & 0xFF;
      buffer[index++] = ais[i] & 0xFF;
    }

    for (int i = 0; i < 4; i++) {
      buffer[index++] = (aos[i] >> 8) & 0xFF;
      buffer[index++] = aos[i] & 0xFF;
    }
  }

  void sendBuffer() {
    DisplaySerial.write(buffer, sizeof(buffer));
  }

  ~DisplayClient() {}
};

extern DisplayClient displayClient;

#endif