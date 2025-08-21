#ifndef SerialRecv_H
#define SerialRecv_H

class SerialRecv {
private:
  const uint8_t START_MARKER = 0xFF;  // not included in buffer
  const uint8_t FRAME_SIZE = 9;       // START_MARKER + 7 + checksum
  bool isReceiving = false;
  uint8_t byteIdx = 0;
  uint8_t buffer[8];

public:
  uint16_t values[5];

  SerialRecv(){};

  void listen() {
    while (Serial2.available() > 0) {
      uint8_t receivedByte = Serial2.read();

      if (receivedByte == START_MARKER) {
        this->isReceiving = true;
        this->byteIdx = 0;
      }

      if (this->isReceiving) {
        this->buffer[byteIdx++] = receivedByte;

        if (this->byteIdx == FRAME_SIZE) {
          this->isReceiving = false;
          this->byteIdx = 0;

          uint8_t checksum = 0;
          for (uint8_t i = 1; i < FRAME_SIZE - 1; i++) {
            checksum += this->buffer[i];
          }

          Serial.println(this->buffer[0]);
          Serial.println(this->buffer[1]);
          Serial.println(this->buffer[2]);
          Serial.println(this->buffer[3]);
          Serial.println(this->buffer[4]);
          Serial.println(this->buffer[5]);
          Serial.println(this->buffer[6]);
          Serial.println(this->buffer[7]);
          Serial.println(this->buffer[8]);
          Serial.println(checksum);

          if (checksum == buffer[FRAME_SIZE - 1]) {
            this->values[0] = buffer[1];
            this->values[1] = buffer[2] | (buffer[3] << 8);
            this->values[2] = buffer[4] | (buffer[5] << 8);
            this->values[3] = buffer[6];
            this->values[4] = buffer[7];
          }

          Serial.print(this->values[0]);
          Serial.print(", ");
          Serial.print(this->values[1]);
          Serial.print(", ");
          Serial.print(this->values[2]);
          Serial.print(", ");
          Serial.print(this->values[3]);
          Serial.print(", ");
          Serial.print(this->values[4]);
          Serial.println();
        }
      }
    }
  }

  ~SerialRecv(){};
};

extern SerialRecv serialRecv;

#endif