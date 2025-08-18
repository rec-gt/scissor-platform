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
  }

  void sendBuffer() {
  }

  ~DisplayClient() {}
};

extern DisplayClient displayClient;

#endif