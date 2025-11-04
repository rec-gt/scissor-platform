#include "Globals.h"
#include "NBIoT.h"

#ifndef PowerMeter_H
#define PowerMeter_H

class PowerMeter {
private:
  byte slaveId;
  unsigned long prevMillis = millis();

  uint32_t holdingRegisterValues[17] = {};
  char* holdingRegisterDescription[17] = {
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
    byte cnt = 0;
    if (millis() - this->prevMillis > 1000) {
      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 0, 6);
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 8, 6);
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 16, 6);
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 30, 2);
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();


      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 54, 2);
      uint16_t HI = mbClient.read();
      uint16_t LO = mbClient.read();
      Serial.println((float)(HI | LO));

      for (int i = 16 - 1; i >= 0; i--) {  // Loop from most significant bit to least significant
        Serial.print(bitRead(HI, i));
      }
      for (int i = 16 - 1; i >= 0; i--) {  // Loop from most significant bit to least significant
        Serial.print(bitRead(LO, i));
      }
      // holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();


      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 70, 2);
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();


      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 506, 2);
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 1400, 6);
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();

      delay(2);

      mbClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 3430, 6);
      mbClient.read();
      mbClient.read();
      mbClient.read();
      mbClient.read();
      holdingRegisterValues[cnt++] = mbClient.read() << 16 | mbClient.read();

      delay(2);

      Serial.println(mbClient.lastError());

      Serial.println();

      for (byte i = 0; i < 17; i++) {
        Serial.print(holdingRegisterDescription[i]);
        Serial.print(" : ");
        Serial.println(holdingRegisterValues[i]);
      }

      this->prevMillis = millis();
    }
  }
};

#endif
