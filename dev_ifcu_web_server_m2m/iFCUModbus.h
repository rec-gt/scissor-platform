#ifndef IFCU_MODBUS_H
#define IFCU_MODBUS_H

#include <ModbusMaster.h>
#define RXD2 16
#define TXD2 17

ModbusMaster node;
uint8_t result;

class iFCUModbus {
private:
  byte slaveId = 31;

public:
  void init() {
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    node.begin(this->slaveId, Serial2);
  }

  void loop() {
    result = node.readInputRegisters(30000, IR_SIZE);
    if (result == node.ku8MBSuccess) {
      for (int i = 0; i < IR_SIZE; i++) {
        IR_DATABASE[i] = node.getResponseBuffer(i);
        Serial.println(IR_DATABASE[i]);
      }
    } else {
      Serial.println("Cannot Fetch Data");
    }
    delay(1000);
  }
};

extern iFCUModbus ifcuModbus;

#endif