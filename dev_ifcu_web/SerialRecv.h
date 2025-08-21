#ifndef SerialRecv_H
#define SerialRecv_H

class SerialRecv {
private:
  const uint8_t START_MARKER = 0xFF;
  const uint8_t FRAME_SIZE = 8;  // START_MARKER + 7
  static bool isReceiving = false;
  static uint8_t byteIdx = 0;
  static uint8_t buffer[FRAME_SIZE];

public:
  byte value[7];

  SerialRecv(){};

  void listen() {
    while (Serial2.available() > 0) {
      uint8_t receivedByte = Serial2.read();

      if (this->isReceiving) {
        this->buffer[byteIdx++] = receivedByte;

        if (this->byteIdx == FRAME_SIZE) {
          isReceiving = false;
          byteIdx = 0;

          // Validate checksum
          uint8_t checksum = 0;
          for (uint8_t i = 0; i < 7; i++) {
            checksum += buffer[i];
          }

          if (checksum == buffer[7]) {  // Compare calculated checksum with received
            values[0] = buffer[0];
            values[1] = buffer[1] | (buffer[2] << 8);
            values[2] = buffer[3] | (buffer[4] << 8);
            values[3] = buffer[5];
            values[4] = buffer[6];
          } else {
            Serial.println("Checksum failed! Discarding frame...");
          }
        }
      } else if (receivedByte == START_MARKER) {
        this->isReceiving = true;
        this->byteIdx = 0;
      }
    }
  }

  ~SerialRecv(){};
};



#endif