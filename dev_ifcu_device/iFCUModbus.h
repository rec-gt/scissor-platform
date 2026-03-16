#ifndef IFCU_MODBUS_H
#define IFCU_MODBUS_H

#include "Globals.h"

#define RXD2 16
#define TXD2 17

constexpr size_t arrayLength = 4 + 1;
uint16_t jsArray[arrayLength];

uint8_t result;

class iFCUModbus {
private:
  uint16_t prevMillis = millis();
  byte errMsg = 0;

  // bool compareData() {}

  void copyArr(uint16_t *arr1, uint16_t *arr2, size_t size) {
    for (size_t i = 0; i < size; i++) {
      arr2[i] = arr1[i];
    }
  }

  void freeQueue() {
    QUEUE = F("");
  }

public:
  void init() {
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    mbNode.begin(IFCU_SLAVE_ID, Serial2);
    delay(1000);
  }



  void handleSendIR() {
    result = mbNode.readInputRegisters(30000, IR_SIZE);
    if (result == mbNode.ku8MBSuccess) {
      for (size_t i = 0; i < IR_SIZE; i++) {
        IR_DATABASE[i] = mbNode.getResponseBuffer(i);
      }

      for (size_t i = 0; i < IR_SIZE; i++) {
        Serial.println(IR_DATABASE[i]);
      }

      this->errMsg = 0;
    } else {
      this->errMsg = 1;
      Serial.println("Cannot Fetch Data");
    }
  }

  void readDataFromDevice() {
    result = mbNode.readInputRegisters(30000, HR_SIZE);
    if (result == mbNode.ku8MBSuccess) {
      for (size_t i = 0; i < HR_SIZE; i++) {
        READ_DATA[i] = mbNode.getResponseBuffer(i);
      }

      for (size_t i = 0; i < HR_SIZE; i++) {
        Serial.println(READ_DATA[i]);
      }

      this->errMsg = 0;
    } else {
      this->errMsg = 1;
      Serial.println("Cannot Fetch Device Data");
    }
  }

  void syncWithQueue() {
    uint16_t tmpArr[5] = {
      (READ_DATA[1] & (1 << 6)) ? 1 : 0,
      READ_DATA[3],
      READ_DATA[4],
      READ_DATA[6],
    };

    this->copyArr(tmpArr, TMP_DATA, 5);

    for (int i = 0; i < QUEUE.length(); i++) {
      char c = QUEUE[i];
      Serial.println(c);
      // A = On.
      // B = Off.
      // C = Set Mode to 0.
      // D = Set Mode to 1.
      // E = Set Mode to 2.
      // F = Set Fan Speed to 0.
      // G = Set Fan Speed to 1.
      // H = Set Fan Speed to 2.
      // I = Increase Set Temp.
      // J = Decrease Set Temp.
      if (c == 'A') {
        // TMP_DATA[1] |= (1 << 6);
        TMP_DATA[0] = 1;
      } else if (c == 'B') {
        // TMP_DATA[1] &= ~(1 << 6);
        TMP_DATA[0] = 1;
      } else if (c == 'C') {
        TMP_DATA[1] = 0;
      } else if (c == 'D') {
        TMP_DATA[1] = 1;
      } else if (c == 'E') {
        TMP_DATA[1] = 2;
      } else if (c == 'F') {
        TMP_DATA[2] = 0;
      } else if (c == 'G') {
        TMP_DATA[2] = 1;
      } else if (c == 'H') {
        TMP_DATA[2] = 2;
      } else if (c == 'I') {
        TMP_DATA[3] += 50;
      } else if (c == 'J') {
        TMP_DATA[3] -= 50;
      }
    }

    this->freeQueue();
  }

  void writeDataToDevice() {
    mbNode.setTransmitBuffer(0, WRITE_DATA[1]);
    mbNode.setTransmitBuffer(1, 0);
    mbNode.setTransmitBuffer(2, WRITE_DATA[3]);
    mbNode.setTransmitBuffer(3, WRITE_DATA[4]);
    mbNode.setTransmitBuffer(4, WRITE_DATA[2]);
    result = mbNode.writeMultipleRegisters(40000, 5);
    if (result == mbNode.ku8MBSuccess) {
      this->errMsg = 0;
    } else {
      this->errMsg = 1;
      Serial.println("Cannot Write Data to Device");
    }
  }
};

extern iFCUModbus ifcuModbus;

#endif