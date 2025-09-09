#include "AnalogInput.h"
#include "AnalogOutput.h"

#ifndef DisplayClient_H
#define DisplayClient_H

#define DisplaySerial Serial2

class DisplayClient {
private:
  uint8_t buffer[40];

  uint8_t getChecksum(uint8_t *buffer, uint8_t idx_from, uint8_t idx_to) {
    uint8_t checksum = 0;
    for (uint8_t i = idx_from; i <= idx_to; i++) {
      checksum += buffer[i];
    }
    return checksum;
  }

public:
  DisplayClient(){};

  void setup() {
    DisplaySerial.begin(9600);
  };

  void prepareBuffer(uint8_t nbiotConn, uint8_t nbiotCsq, uint8_t dis, uint8_t dos, AnalogInput *ais, AnalogOutput *aos, uint8_t aiMappingMode) {
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

    Serial.print("aiMappingMode: ");
    Serial.println(aiMappingMode);

    this->buffer[idx++] = aiMappingMode;

    this->buffer[idx++] = getChecksum(this->buffer, 1, 37);

    this->buffer[idx++] = 0x5D;  // ']'
  }

  void sendBuffer() {
    DisplaySerial.write(this->buffer, sizeof(this->buffer));
    DisplaySerial.flush();
  }

  void debug() {
    for (size_t i = 0; i < 39; i++) {
      Serial.print(this->buffer[i]);
      Serial.print(", ");
    }
    Serial.println();
  }

  ~DisplayClient() {}
};

extern DisplayClient displayClient;

#endif