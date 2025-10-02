#include <SoftwareSerial.h>

#ifndef DisplayClient_H
#define DisplayClient_H

#define DISPLAY_BUFFER_SIZE 41

SoftwareSerial DisplaySerial(20, 21);  // RX, TX

class DisplayClient {
private:
  uint8_t buffer[DISPLAY_BUFFER_SIZE];

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

  void prepareBuffer(uint8_t nbiotConn, uint8_t nbiotCsq, uint8_t dis, uint8_t dos, LaserSensor *ais, LaserSensor *aos, uint16_t aiMappingMode) {
    uint8_t idx = 0;

    this->buffer[idx++] = 0x5B;  // '['

    this->buffer[idx++] = nbiotConn;
    this->buffer[idx++] = nbiotCsq;
    this->buffer[idx++] = dis;
    this->buffer[idx++] = dos;

    for (int i = 0; i < 12; i++) {
      this->buffer[idx++] = (ais[i].reading >> 8) & 0xFF;
      this->buffer[idx++] = ais[i].reading & 0xFF;
    }

    for (int i = 0; i < 4; i++) {
      this->buffer[idx++] = (aos[i].reading >> 8) & 0xFF;
      this->buffer[idx++] = aos[i].reading & 0xFF;
    }

    this->buffer[idx++] = (aiMappingMode >> 8) & 0xFF;

    this->buffer[idx++] = aiMappingMode & 0xFF;

    this->buffer[idx++] = getChecksum(this->buffer, 1, idx - 1);

    this->buffer[idx++] = 0x5D;  // ']'
  }

  void sendBuffer() {
    DisplaySerial.write(this->buffer, sizeof(this->buffer));
    DisplaySerial.flush();
  }

  void debug() {
    for (size_t i = 0; i < DISPLAY_BUFFER_SIZE; i++) {
      Serial.print(this->buffer[i]);
      Serial.print(", ");
    }
    Serial.println();
  }

  ~DisplayClient() {}
};

extern DisplayClient displayClient;

#endif