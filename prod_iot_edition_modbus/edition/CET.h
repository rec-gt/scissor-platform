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
  // { INPUT_REGISTERS, i, n },
  // { COILS, i, n },
  // { DISCRETE_INPUTS, i, n },

  { HOLDING_REGISTERS, 0, 6 },   // 根據CET Modbus Table, HR addr 從 0 讀取, 連續讀取6個數據
  { HOLDING_REGISTERS, 46, 2 },  // 根據CET Modbus Table, HR addr 從 46 讀取, 連續讀取2個數據
};

/*=== End of config ===*/

uint8_t mbNumOfConfig = 0;
uint8_t mbNumOfConfigSwitch = 0;
uint8_t mbNumOfData = 0;
uint8_t mbNumOfDataSwitch = 0;
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
      this->readIn500ms();
      // this->debug();
    }
  }

  void readIn500ms() {  // it is a loop
    if (mbNumOfConfigSwitch >= mbNumOfConfig) {
      mbNumOfConfigSwitch = 0;
      return;
    }

    Serial.print("Switch Idx: ");
    Serial.println(mbNumOfConfigSwitch);

    uint8_t mbDataType = mbReadConfig[mbNumOfConfigSwitch][0];
    uint8_t mbDataStartIdx = mbReadConfig[mbNumOfConfigSwitch][1];
    uint8_t mbDataRange = mbReadConfig[mbNumOfConfigSwitch][2];

    mbRtuClient.requestFrom(SLAVE_ID, mbDataType, mbDataStartIdx, mbDataRange);

    for (uint8_t j = 0; j < mbDataRange; j++) {
      Serial.println(mbRtuClient.read());
    }

    mbNumOfConfigSwitch++;
  }

  // void readIn1000ms() {
  //   mbRtuClient.requestFrom(SLAVE_ID, HOLDING_REGISTERS, 0, 6);
  //   holdingRegisterValues[0] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
  //   holdingRegisterValues[1] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());
  //   holdingRegisterValues[2] = IEEEfloat(((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read());

  //   mbRtuClient.requestFrom(SLAVE_ID, HOLDING_REGISTERS, 46, 2);
  //   holdingRegisterValues[3] = (((uint32_t)mbRtuClient.read() << 16) | mbRtuClient.read()) * 0.01;
  // }

  // void debug() {
  //   Serial.println(F("\r\n>>> Print Data\r\n"));

  //   for (byte i = 0; i < PARAMETERS_SIZE; i++) {
  //     Serial.print(holdingRegisterDescription[i]);
  //     Serial.print(F(": "));
  //     Serial.println(holdingRegisterValues[i], 4);
  //   }
  // }
};

#endif
