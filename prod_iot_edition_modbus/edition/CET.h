#include <stdint.h>
#include "./SubGlobals.h"

#ifndef CET_H
#define CET_H

/*=== Derek, please config modbus here ===*/
constexpr uint16_t SLAVE_ID = 12;                  // target device 的 slave id
constexpr uint16_t MB_BAUD_RATE = 9600;            // serial 的 baud rate
constexpr uint16_t MB_SERIAL_CONFIG = SERIAL_8E1;  // e.g., SERIAL_8E1, SERIAL_8N1
constexpr uint8_t MB_DATA_SIZE = 100;

uint8_t mbReadConfig[MB_DATA_SIZE][3] = {
  // format: { modbus data type, modbus data address, (default 2) number of consecutive data }
  // 通常read得最多的是HOLDING_REGISTERS，其他少機會用到
  // 有連續的data盡量在最右邊的位置寫下有多少個連續數據，這是性能考量
  // { HOLDING_REGISTERS, 0, 6 },   // 等於從address 0開始，連續讀取6個數據，也就是read: 0, 1, 2, 3, 4, 5
  // { HOLDING_REGISTERS, 46, 2 },  // 等於從address 46開始，連續讀取2個數據，也就是read: 46, 47
  // { INPUT_REGISTERS, i, n },     // 根據需求及產品Modbus Table説明而定，請自行研究
  // { COILS, i, n },               // 根據需求及產品Modbus Table説明而定，請自行研究
  // { DISCRETE_INPUTS, i, n },     // 根據需求及產品Modbus Table説明而定，請自行研究

  { HOLDING_REGISTERS, 0, 6 },   // 根據CET Modbus Table, HR addr 從 0 讀取, 連續讀取6個數據
  { HOLDING_REGISTERS, 46, 2 },  // 根據CET Modbus Table, HR addr 從 46 讀取, 連續讀取2個數據
};

/*=== End of config ===*/

uint8_t mbNumOfConfig = 0;
uint8_t mbNumOfConfigSwitch = 0;
uint8_t mbNumOfData = 0;
uint8_t mbDataPtr = 0;
uint32_t mbHrData[MB_DATA_SIZE] = {};

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

    for (uint8_t i = 0; i < 100; i++) {
      uint8_t numCnt = mbReadConfig[i][2];
      if (numCnt > 0) {
        mbNumOfConfig++;
        mbNumOfData += numCnt;
      }
    }

    Serial.println(mbNumOfConfig);
    Serial.println(mbNumOfData);
  }

  void loop() {
    if (mbTimer.autoTimeout(1000)) {
      this->autoFill();
      // this->debug();
    }
  }

  void autoFill() {  // it is a loop
    if (mbNumOfConfigSwitch >= mbNumOfConfig) {
      /*=== End of an epoch ===*/
      mbDataPtr = 0;
      mbNumOfConfigSwitch = 0;
      return;
    }

    uint8_t mbDataType = mbReadConfig[mbNumOfConfigSwitch][0];
    uint8_t mbDataStartIdx = mbReadConfig[mbNumOfConfigSwitch][1];
    uint8_t mbDataRange = mbReadConfig[mbNumOfConfigSwitch][2];

    mbRtuClient.requestFrom(SLAVE_ID, mbDataType, mbDataStartIdx, mbDataRange);

    for (uint8_t j = 0; j < mbDataRange; j++) {
      mbHrData[mbDataPtr] = mbRtuClient.read();
      mbDataPtr++;
    }

    mbNumOfConfigSwitch++;
  }

  void debug() {
    Serial.print(F("\r\n>>> Print Data\r\n"));

    for (uint8_t i = 0; i < mbNumOfData; i++) {
      Serial.print(i);
      Serial.print(F(": "));
      Serial.println(mbHrData[i]);
    }
  }
};

#endif
