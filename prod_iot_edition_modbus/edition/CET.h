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
constexpr uint8_t MB_READ_EPOCH_MAX = (MB_DATA_SIZE + MB_DATA_BATCH_SIZE - 1) / MB_DATA_BATCH_SIZE;
uint8_t MB_READ_EPOCH_IDX = 0;

uint8_t mbReadConfig[MB_DATA_SIZE][3] = {
  // format: { modbus data type, modbus data address, (default 1) number of consecutive data }
  // 通常read得最多的是HOLDING_REGISTERS，其他少機會用到
  // 有連續的data盡量在最右邊的位置寫下有多少個連續數據，這是性能考量
  { HOLDING_REGISTERS, 1, 1 },   // 等於從address 1開始，連續讀取1個數據，也就是read: 1
  { HOLDING_REGISTERS, 2, 1 },   // 等於從address 2開始，連續讀取1個數據，也就是read: 2
  { HOLDING_REGISTERS, 3, 4 },   // 等於從address 3開始，連續讀取4個數據，也就是read: 3, 4, 5, 6
  { HOLDING_REGISTERS, 7, 1 },   // 等於從address 7開始，連續讀取1個數據，也就是read: 7
  { HOLDING_REGISTERS, 30, 2 },  // 等於從address 30開始，連續讀取2個數據，也就是read: 30, 31

  // { INPUT_REGISTERS, 1, 1 },
  // { INPUT_REGISTERS, 2, 1 },
  // { INPUT_REGISTERS, 3, 1 },
  // { COILS, 1, 1 },
  // { COILS, 2, 1 },
  // { COILS, 3, 1 },
  // { DISCRETE_INPUTS, 1, 1 },
  // { DISCRETE_INPUTS, 2, 1 },
  // { DISCRETE_INPUTS, 3, 1 },
};

uint32_t mbHrData[MB_DATA_SIZE] = {};

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

  void readIn1000ms() {
    mbRtuClient.requestFrom(SLAVE_ID, HOLDING_REGISTERS, startIdx, MB_DATA_BATCH_SIZE);

    for (uint8_t i = startIdx; i < MB_DATA_BATCH_SIZE + startIdx; i++) {
      mbHrData[i] = mbRtuClient.read()
    }

    for (uint8_t i = startIdx; i < MB_DATA_BATCH_SIZE + startIdx; i++) {
      mbHrData[i] = mbRtuClient.read()
    }
  }

  // void readIn1000ms() {
  //   mbRtuClient.requestFrom(SLAVE_ID, HOLDING_REGISTERS, 0, 6);
  //   holdingRegisterValues[0] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
  //   holdingRegisterValues[1] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
  //   holdingRegisterValues[2] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());

  //   mbRtuClient.requestFrom(SLAVE_ID, HOLDING_REGISTERS, 46, 2);
  //   holdingRegisterValues[3] = (((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read()) * 0.01;
  // }

  // void showData() {
  //   Serial.println(F("\r\n>>> Print Data\r\n"));

  //   for (byte i = 0; i < PARAMETERS_SIZE; i++) {
  //     Serial.print(holdingRegisterDescription[i]);
  //     Serial.print(F(": "));
  //     Serial.println(holdingRegisterValues[i], 4);
  //   }
  // }
};

#endif
