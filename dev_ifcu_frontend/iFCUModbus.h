#ifndef IFCU_MODBUS_H
#define IFCU_MODBUS_H

#include <ModbusMaster.h>
#include "Globals.h"
#define RXD2 16
#define TXD2 17

ModbusMaster node;
uint8_t result;

class iFCUModbus {
private:
  byte slaveId = 31;
  uint32_t prevMillis = millis();

public:
  void init() {
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    node.begin(this->slaveId, Serial2);
  }

  void loop() {
    uint32_t currMillis = millis();

    if (currMillis - this->prevMillis >= 1000) {
      this->prevMillis = currMillis;

      result = node.readInputRegisters(30000, IR_SIZE);
      if (result == node.ku8MBSuccess) {
        for (size_t i = 0; i < IR_SIZE; i++) {
          IR_DATABASE[i] = node.getResponseBuffer(i);
          Serial.println(IR_DATABASE[i]);
        }
      } else {
        Serial.println("Cannot Fetch Data");
      }
    }
  }
};

extern iFCUModbus ifcuModbus;

#endif