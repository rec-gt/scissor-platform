#include <stdint.h>
#include "./SubGlobals.h"

#ifndef CET_H
#define CET_H

/*=== Derek, please config modbus here ===*/
constexpr uint16_t SLAVE_ID = 1;
constexpr uint16_t MB_BAUD_RATE = 9600;
constexpr uint16_t MB_SERIAL_CONFIG = SERIAL_8E1;  // e.g., SERIAL_8E1, SERIAL_8N1
constexpr uint8_t MB_DATA_SIZE = 100;
constexpr uint8_t MB_DATA_BATCH_SIZE = 20;
constexpr uint8_t MB_READ_EPOCH = (MB_DATA_SIZE + MB_DATA_BATCH_SIZE - 1) / MB_DATA_BATCH_SIZE;
uint32_t mbData[MB_DATA_SIZE] = {};
/*=== End of config ===*/

class CET {
private:
  float IEEEfloat(uint32_t val) {
    union {
      uint32_t i;
      float f;
    } u;
    u.i = val;
    return u.f;
  }

public:
  CET(void) {}

  void init() {
    if (!mbRtuClient.begin(MB_BAUD_RATE, MB_SERIAL_CONFIG)) {
      Serial.println(F("Failed to start Modbus RTU Client!"));
      while (1) {};
    }
  }

  void loop() {
    if (deviceTimer.autoTimeout(1000)) {
      this->readIn1000ms();
      this->showData();
    }
  }

  void readIn1000ms(uint8_t startIdx, uint8_t num) {
    mbRtuClient.requestFrom(SLAVE_ID, HOLDING_REGISTERS, startIdx, num);
    for (uint8_t i = f; i < t; i++) {
      mbRtuClient.read()
    }
    holdingRegisterValues[0] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
    holdingRegisterValues[1] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
    holdingRegisterValues[2] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());

    mbRtuClient.requestFrom(SLAVE_ID, HOLDING_REGISTERS, 46, 2);
    holdingRegisterValues[3] = (((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read()) * 0.01;
  }

  void readIn1000ms() {
    mbRtuClient.requestFrom(SLAVE_ID, HOLDING_REGISTERS, 0, 6);
    holdingRegisterValues[0] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
    holdingRegisterValues[1] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
    holdingRegisterValues[2] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());

    mbRtuClient.requestFrom(SLAVE_ID, HOLDING_REGISTERS, 46, 2);
    holdingRegisterValues[3] = (((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read()) * 0.01;
  }

  void showData() {
    Serial.println(F("\r\n>>> Print Data\r\n"));

    for (byte i = 0; i < PARAMETERS_SIZE; i++) {
      Serial.print(holdingRegisterDescription[i]);
      Serial.print(F(": "));
      Serial.println(holdingRegisterValues[i], 4);
    }
  }
};

#endif
