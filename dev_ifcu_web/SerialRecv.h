#ifndef SerialRecv_H
#define SerialRecv_H

class SerialRecv {
private:
  const uint8_t START_MARKER = 0xFF;  // not included in frame
  const uint8_t FRAME_SIZE = 8;       // 7 + checksum
  const uint8_t BYTES_SIZE = 7;
  bool isReceiving = false;
  uint8_t byteIdx = 0;
  uint8_t buffer[8];

public:
  byte values[5];

  SerialRecv(){};

  void listen() {
    while (Serial2.available() > 0) {
      uint8_t receivedByte = Serial2.read();

      if (this->isReceiving) {
        this->buffer[byteIdx++] = receivedByte;

        Serial.println(receivedByte);

        if (this->byteIdx == FRAME_SIZE) {
          this->isReceiving = false;
          this->byteIdx = 0;

          uint8_t checksum = 0;
          for (uint8_t i = 0; i < BYTES_SIZE; i++) {
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


          if (checksum == buffer[FRAME_SIZE - 1]) {
            this->values[0] = buffer[0];
            this->values[1] = buffer[1] | (buffer[2] << 8);
            this->values[2] = buffer[3] | (buffer[4] << 8);
            this->values[3] = buffer[5];
            this->values[4] = buffer[6];
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
      } else if (receivedByte == START_MARKER) {
        this->isReceiving = true;
        this->byteIdx = 0;
      }
    }
  }

  ~SerialRecv(){};
};

extern SerialRecv serialRecv;

#endif