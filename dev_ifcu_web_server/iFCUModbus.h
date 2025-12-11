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
  byte slaveId = 0;
  uint16_t prevMillis = millis();

public:
  void init() {
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    node.begin(this->slaveId, Serial2);
  }

  void loop() {
    uint16_t currMillis = millis();

    if (currMillis - this->prevMillis >= 1000) {
      this->prevMillis = currMillis;

      node.setTransmitBuffer(0, 0);
      node.setTransmitBuffer(2, 0);
      node.setTransmitBuffer(3, 0);

      result = node.writeMultipleRegisters(40000, 3);
      if (result == node.ku8MBSuccess) {
        Serial.println("Success");
      } else {
        Serial.println("Fail to fetch data");
      }
    }
  }
};

extern iFCUModbus ifcuModbus;

#endif