#ifndef SerialRecv_H
#define SerialRecv_H

class SerialRecv {
private:
  const uint8_t START_MARKER = 0x5B;  // '['
  const uint8_t END_MARKER = 0x5D;    // ']'
  uint8_t buffer[22];                 // START_MARKER + 7 + checksum + END_MARKER
  uint8_t idx = 0;
  bool isReceiving = false;

  uint8_t getChecksum(uint8_t buffer*, uint8_t idx_from, uint8_t idx_to) {
    uint8_t checksum = 0;
    for (uint8_t i = 1; i < FRAME_SIZE - 1; i++) {
      checksum += buffer[i];
    }
    return checksum;
  }

public:
  uint8_t nbiotConn;
  uint8_t nbiotCsq;
  uint8_t dis;
  uint8_t dos;
  uint16_t ais[12];
  uint16_t aos[4];

  SerialRecv(){};

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

          uint8_t payloadChecksum = this->buffer[21];
          uint8_t calculatedChecksum = this->getChecksum(this->buffer, 1, 20);

          if (payloadChecksum == calculatedChecksum) {
            this->extractValues();
          }
        }

        // if (this->idx == FRAME_SIZE) {
        //   this->isReceiving = false;
        //   this->idx = 0;

        //   uint8_t checksum = 0;
        //   for (uint8_t i = 1; i < FRAME_SIZE - 1; i++) {
        //     checksum += this->buffer[i];
        //   }

        //   if (checksum == buffer[FRAME_SIZE - 1]) {
        //     this->values[0] = buffer[1];
        //     this->values[1] = buffer[2] | (buffer[3] << 8);
        //     this->values[2] = buffer[4] | (buffer[5] << 8);
        //     this->values[3] = buffer[6];
        //     this->values[4] = buffer[7];
        //   }
        // }
      }
    }
  }

  void extractValues() {
    uint8_t idx = 1;

    this->nbiotConn = buffer[idx++];
    this->nbiotCsq = buffer[idx++];
    this->dis = buffer[idx++];
    this->dos = buffer[idx++];

    for (size_t i = 0; i < 12; i++) {
      this->ais[i] = (buffer[idx++] << 8) & buffer[idx++];
    }

    for (size_t i = 0; i < 4; i++) {
      this->aos[i] = (buffer[idx++] << 8) & buffer[idx++];
    }
  }

  void debug() {
    Serial.println(this->nbiotConn);
    Serial.println(this->nbiotCsq);
    Serial.println(this->dis);
    Serial.println(this->dos);
    
    for (size_t i = 0; i < 12; i++) {
      Serial.print(this->ais[i]);
      Serial.print(", ");
    }
    Serial.println();

    for (size_t i = 0; i < 4; i++) {
      Serial.print(this->aos[i]);
      Serial.print(", ");
    }
    Serial.println();
  }

  ~SerialRecv(){};
};

extern SerialRecv serialRecv;

#endif