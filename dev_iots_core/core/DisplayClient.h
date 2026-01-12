#include "./AnalogInput.h"
#include "./AnalogOutput.h"
#include "./Globals.h"

#ifndef DisplayClient_H
#define DisplayClient_H

#define DisplaySerial Serial2
#define DISPLAY_BUFFER_SIZE 55

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

  uint32_t prevMillis;

public:
  DisplayClient(){};

  void setup() {
    DisplaySerial.begin(9600);
  };

  void loop() {
    uint32_t currMillis = millis();

    if (currMillis - this->prevMillis > 2000) {
      this->prepareBuffer();
      this->sendBuffer();
      this->prevMillis = currMillis;
    }
  }

  void prepareBuffer() {
    {
      int csq = iotCSQ.toInt();

      uint8_t idx = 0;

      this->buffer[idx++] = 0x5B;  // '['

      this->buffer[idx++] = iotConnState;
      this->buffer[idx++] = csq;
      this->buffer[idx++] = DIPayload;
      this->buffer[idx++] = DOPayload;

      for (int i = 0; i < AI_NUMS; i++) {
        this->buffer[idx++] = (analogInputs[i].value >> 8) & 0xFF;
        this->buffer[idx++] = analogInputs[i].value & 0xFF;
      }

      for (int i = 0; i < AO_NUMS; i++) {
        this->buffer[idx++] = (analogOutputs[i].value >> 8) & 0xFF;
        this->buffer[idx++] = analogOutputs[i].value & 0xFF;
      }

      for (int i = 0; i < 16; i++) {
        this->buffer[idx++] = iotIMEI.charAt(i);
      }

      uint8_t currIdx = idx;
      this->buffer[idx++] = getChecksum(this->buffer, 1, currIdx - 1);

      this->buffer[idx++] = 0x5D;  // ']'
    }
  }

  void sendBuffer() {
    DisplaySerial.write(this->buffer, sizeof(this->buffer));
    DisplaySerial.flush();
  }

  void debug() {
    for (size_t i = 0; i < DISPLAY_BUFFER_SIZE; i++) {
      Serial.print(this->buffer[i]);
      Serial.print(F(", "));
    }
    Serial.println();
  }

  ~DisplayClient() {}
};

extern DisplayClient displayClient;

#endif