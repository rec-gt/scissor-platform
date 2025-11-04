#include "Globals.h"
#include "NBIoT.h"

#ifndef PowerMeter_H
#define PowerMeter_H

class PowerMeter {
private:
  byte slaveId;
  unsigned long prevMillis = millis();

  uint32_t holdingRegisterValues[100] = {};
  char* holdingRegisterDescription[100] = {
    (char*)"Uan",
    (char*)"Ubn",
    (char*)"Ucn",
    (char*)"Uab",
    (char*)"Ubc",
    (char*)"Uca",
    (char*)"Ia",
    (char*)"Ib",
    (char*)"Ic",
    (char*)"P Total",
    (char*)"PF Total",
    (char*)"In (Calculated)",
    (char*)"kWh Total",
    (char*)"Ia THD",
    (char*)"Ib THD",
    (char*)"Ic THD",
    (char*)"S Total",
  };

public:
  PowerMeter(byte slaveId)
    : slaveId(slaveId){};

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

      Serial.println();

      this->prevMillis = millis();
    }
  }
};

#endif
