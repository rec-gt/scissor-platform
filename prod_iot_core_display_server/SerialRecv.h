#ifndef SerialRecv_H
#define SerialRecv_H
#define PeripheralSerial Serial
#define DISPLAY_BUFFER_SIZE 56
// #define DISPLAY_BUFFER_SIZE 41

class SerialRecv {
private:
  const uint8_t START_MARKER = 0x5B;    // '['
  const uint8_t END_MARKER = 0x5D;      // ']'
  uint8_t buffer[DISPLAY_BUFFER_SIZE];  // START_MARKER + 8 + checksum + END_MARKER
  uint8_t idx = 0;
  bool isReceiving = false;

  uint8_t getChecksum(uint8_t *buffer, uint8_t idx_from, uint8_t idx_to) {
    uint8_t checksum = 0;
    for (uint8_t i = idx_from; i <= idx_to; i++) {
      checksum += buffer[i];
    }
    return checksum;
  }

public:
  uint8_t iotConn;
  uint8_t iotCsq;
  uint8_t iotApp;
  uint8_t dis;
  uint8_t dos;
  uint16_t ais[12];
  uint16_t aos[4];
  char imei[16] = {};

  void listen() {
    while (PeripheralSerial.available() > 0) {
      uint8_t rb = PeripheralSerial.read();

      if (rb == START_MARKER) {
        this->isReceiving = true;
        this->idx = 0;
      }

      if (this->isReceiving) {
        this->buffer[idx++] = rb;

        if (rb == END_MARKER) {
          this->isReceiving = false;
          this->idx = 0;

          uint8_t payloadChecksum = this->buffer[DISPLAY_BUFFER_SIZE - 2];
          uint8_t calculatedChecksum = this->getChecksum(this->buffer, 1, DISPLAY_BUFFER_SIZE - 3);
          if (payloadChecksum == calculatedChecksum) {
            this->extractValues();
          } else {
            Serial.println(F("wtf"));
          }
        }
      }
    }
  }

  void extractValues() {
    uint8_t idx = 1;

    this->iotConn = buffer[idx++];
    this->iotCsq = buffer[idx++];
    this->iotApp = buffer[idx++];
    this->dis = buffer[idx++];
    this->dos = buffer[idx++];

    for (size_t i = 0; i < 12; i++) {
      this->ais[i] = (buffer[idx++] << 8) | buffer[idx++];
    }

    for (size_t i = 0; i < 4; i++) {
      this->aos[i] = (buffer[idx++] << 8) | buffer[idx++];
    }

    for (size_t i = 0; i < 16; i++) {
      this->imei[i] = buffer[idx++];
    }
  }

  // void debug() {
  //   Serial.println(this->iotConn);
  //   Serial.println(this->iotCsq);
  //   Serial.println(this->iotApp);
  //   Serial.println(this->dis);
  //   Serial.println(this->dos);

  //   for (size_t i = 0; i < 12; i++) {
  //     Serial.print(this->ais[i]);
  //     Serial.print(", ");
  //   }
  //   Serial.println();

  //   for (size_t i = 0; i < 4; i++) {
  //     Serial.print(this->aos[i]);
  //     Serial.print(", ");
  //   }

  //   Serial.println(this->aiModes);

  //   Serial.println();
  // }

  ~SerialRecv(){};
};

extern SerialRecv serialRecv;

#endif