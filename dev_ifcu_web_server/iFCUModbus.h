#ifndef IFCU_MODBUS_H
#define IFCU_MODBUS_H

#include <ModbusMaster.h>
#include "Globals.h"
#define RXD2 16
#define TXD2 17


uint8_t result;

class iFCUModbus {
private:
  uint16_t prevMillis = millis();

public:
  void init() {
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    mbNode.begin(IFCU_SLAVE_ID, Serial2);
  }

  // void loop() {
  //   uint16_t currMillis = millis();

  //   if (currMillis - this->prevMillis >= 1000) {
  //     this->prevMillis = currMillis;

  //     result = mbNode.readInputRegisters(30000, IR_SIZE);
  //     if (result == mbNode.ku8MBSuccess) {
  //       for (size_t i = 0; i < IR_SIZE; i++) {
  //         IR_DATABASE[i] = mbNode.getResponseBuffer(i);
  //         Serial.println(IR_DATABASE[i]);
  //       }
  //     } else {
  //       Serial.println("Cannot Fetch Data");
  //     }
  //   }
  // }
};

extern iFCUModbus ifcuModbus;

#endif