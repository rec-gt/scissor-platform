#include "Globals.h"
#include "NBIoT.h"

#ifndef PowerMeter_H
#define PowerMeter_H

class PowerMeter {
private:
  byte slaveId;
  unsigned long prevMillis = millis();

  static constexpr long HOLDING_REGISTERS_START_ADDRESS = 40000;
  static constexpr byte HOLDING_REGISTER_COUNT = 13;
  long holdingRegisterValues[HOLDING_REGISTER_COUNT] = {};

public:
  PowerMeter(byte slaveId)
    : slaveId(slaveId){};

  uint16_t cnt = 0;

  // void listen() {
  //   while (Serial3.available() > 0) {
  //     char c = Serial3.read();

  //     Serial.print(c);

  //     if (c != '\r' && c != '\n') {
  //       serialRes += c;
  //     }

  //     if (c == '\r') {
  //     }
  //   }
  // }

  void read() {
    if (millis() - this->prevMillis > 1000) {
      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 0, 6);
      for (uint16_t i = 0; i < 6; i++) {
        Serial.println(mbClient.read());
      }

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 8, 6);
      for (uint16_t i = 0; i < 6; i++) {
        Serial.println(mbClient.read());
      }

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 16, 6);
      for (uint16_t i = 0; i < 6; i++) {
        Serial.println(mbClient.read());
      }

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 30, 2);
      for (uint16_t i = 0; i < 2; i++) {
        Serial.println(mbClient.read());
      }

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 54, 2);
      for (uint16_t i = 0; i < 2; i++) {
        Serial.println(mbClient.read());
      }

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 70, 2);
      for (uint16_t i = 0; i < 2; i++) {
        Serial.println(mbClient.read());
      }

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 506, 2);
      for (uint16_t i = 0; i < 2; i++) {
        Serial.println(mbClient.read());
      }

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 1400, 6);
      for (uint16_t i = 0; i < 6; i++) {
        Serial.println(mbClient.read());
      }

      delay(2);


      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 3430, 6);
      for (uint16_t i = 0; i < 6; i++) {
        Serial.println(mbClient.read());
      }

      delay(2);

      Serial.println(mbClient.lastError());



      // if (!mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 22, 2)) {
      //   Serial.println(mbClient.lastError());
      // } else {
      //   for (uint16_t i = 0; i < 2; i++) {
      //     Serial.println(mbClient.read());
      //   }
      // }

      // === debug ===
      // for (uint16_t i = 0; i < HOLDING_REGISTER_COUNT; i++) {
      //   Serial.print(this->holdingRegisterValues[i]);
      //   Serial.print(", ");
      // }
      Serial.println();

      this->prevMillis = millis();
    }
  }
};

#endif
