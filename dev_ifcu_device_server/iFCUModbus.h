#ifndef IFCU_MODBUS_H
#define IFCU_MODBUS_H

#include "Globals.h"

#define RXD2 16
#define TXD2 17

class iFCUModbus {
private:
  uint32_t prevMillis = millis();
  bool writeDataChanged = false;
  byte taskSwitch = 0;
  byte unSyncedCnt = 0;

  void copyArr(uint16_t *arr1, uint16_t *arr2, size_t size) {
    for (size_t i = 0; i < size; i++) {
      arr2[i] = arr1[i];
    }
  }

  bool compareArr(uint16_t *arr1, uint16_t *arr2, size_t size) {
    for (size_t i = 0; i < size; i++) {
      if (arr2[i] != arr1[i]) {
        return false;
      }
    }
    return true;
  }

  bool hasQueue() {
    return QUEUE != F("");
  }

  void freeQueue() {
    QUEUE = F("");
  }

  void printModbusData() {
    Serial.println("=== Data read from MODBUS ===");
    for (size_t i = 0; i < IR_SIZE; i++) {
      Serial.println(READ_DATA[i]);
    }
    Serial.println("=== End of read ===");
  }

  void printReadData() {
    Serial.println("=== READ_DATA ===");
    Serial.println((READ_DATA[1] & (1 << 6)) ? 1 : 0);
    Serial.println(READ_DATA[3]);
    Serial.println(READ_DATA[4]);
    Serial.println(READ_DATA[5]);
    Serial.println(READ_DATA[6]);
  }

  void printWriteData() {
    Serial.println("=== WRITE_DATA ===");
    for (size_t i = 0; i < 4; i++) {
      Serial.println(WRITE_DATA[i]);
    }
  }

public:
  void init() {
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    mbNode.begin(IFCU_SLAVE_ID, Serial2);
    delay(100);
  }

  void loop() {
    if (millis() - this->prevMillis >= 250) {
      if (this->taskSwitch == 0) {
        this->readDataFromDevice();
      } else if (this->taskSwitch == 1) {
        this->syncWithQueue();
      } else if (this->taskSwitch == 2) {
        this->checkIsSynced();
      } else if (this->taskSwitch == 3) {
        this->writeDataToDevice();
      }

      if (this->taskSwitch == 3) {
        this->taskSwitch = 0;
      } else {
        this->taskSwitch++;
      }

      this->prevMillis = millis();
    }
  }

  void readDataFromDevice() {
    mbResult = mbNode.readInputRegisters(30000, IR_SIZE);
    if (this->mbSuccess()) {
      for (size_t i = 0; i < IR_SIZE; i++) {
        READ_DATA[i] = mbNode.getResponseBuffer(i);
      }

      /*=== Init WRITE_DATA ===*/
      WRITE_DATA[0] = (READ_DATA[1] & (1 << 6)) ? 1 : 0;  // onOff
      WRITE_DATA[1] = READ_DATA[3];                       // mode
      WRITE_DATA[2] = READ_DATA[4];                       // speed
      WRITE_DATA[3] = READ_DATA[6];                       // setTemp
      WRITE_DATA[4] = READ_DATA[5];                       // roomTemp
      WRITE_DATA[5] = READ_DATA[9];                       // max adjustable setTemp
      WRITE_DATA[6] = READ_DATA[10];                      // min adjustable setTemp
    } else {
      Serial.println("Cannot Fetch Device Data");
    }
  }

  void syncWithQueue() {
    if (!isSynced) {
      return;
    }

    if (this->hasQueue()) {
      Serial.print("Queue consumpted: ");
      Serial.println(QUEUE);
      /*=== Manipulation WRITE_DATA ===*/
      for (int i = 0; i < QUEUE.length(); i++) {
        char c = QUEUE[i];
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
          WRITE_DATA[0] = 1;
        } else if (c == 'B') {
          WRITE_DATA[0] = 0;
        } else if (c == 'C') {
          WRITE_DATA[1] = 0;
        } else if (c == 'D') {
          WRITE_DATA[1] = 1;
        } else if (c == 'E') {
          WRITE_DATA[1] = 2;
        } else if (c == 'F') {
          WRITE_DATA[2] = 0;
        } else if (c == 'G') {
          WRITE_DATA[2] = 1;
        } else if (c == 'H') {
          WRITE_DATA[2] = 2;
        } else if (c == 'I') {
          if (WRITE_DATA[3] + 50 <= WRITE_DATA[5]) {
            WRITE_DATA[3] += 50;
          }
        } else if (c == 'J') {
          if (WRITE_DATA[6] <= WRITE_DATA[3] - 50) {
            WRITE_DATA[3] -= 50;
          }
        }
      }

      /*=== Make WRITE_DATA_CMP for sync ===*/
      this->copyArr(WRITE_DATA, WRITE_DATA_1, WRITE_DATA_SIZE);

      /*=== Finish ===*/
      this->writeDataChanged = true;
      this->freeQueue();
    }
  }

  void checkIsSynced() {
    isSynced = this->compareArr(WRITE_DATA_1, WRITE_DATA_2, WRITE_DATA_SIZE);

    /*=== Protection: Force Sync after 5 conflict ===*/
    if (!isSynced) {
      this->unSyncedCnt++;
      if (this->unSyncedCnt >= 5) {
        this->copyArr(WRITE_DATA_1, WRITE_DATA_2, WRITE_DATA_SIZE);
        this->unSyncedCnt = 0;
      }
    }
  }

  void writeDataToDevice() {
    if (!this->writeDataChanged) {
      return;
    } else {
      this->writeDataChanged = false;

      mbNode.setTransmitBuffer(0, WRITE_DATA[0]);
      mbNode.setTransmitBuffer(2, WRITE_DATA[1]);
      mbNode.setTransmitBuffer(3, WRITE_DATA[2]);
      mbNode.setTransmitBuffer(4, WRITE_DATA[3]);
      mbResult = mbNode.writeMultipleRegisters(40000, 5);
      if (mbResult != mbNode.ku8MBSuccess) {
        Serial.println("Cannot Write Data to Device");
      }
    }
  }

  bool mbSuccess() {
    return mbResult == mbNode.ku8MBSuccess;
  }
};

extern iFCUModbus ifcuModbus;

#endif