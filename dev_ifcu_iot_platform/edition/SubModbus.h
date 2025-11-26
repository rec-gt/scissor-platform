#include "Arduino.h"
#include "HardwareSerial.h"
#ifndef SubModbus_H
#define SubModbus_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class SubModbus {
private:
  byte slaveId = 31;
  uint16_t prevMillis = 0;


public:
  SubModbus(void) {}

  void init() {
    mbRtuClient.begin(9600);
  }

  void loop() {
    this->read();
  }

  void read() {
    uint16_t currMillis = millis();
    if (currMillis - this->prevMillis > 1000) {
      this->prevMillis = currMillis;

      if (!mbRtuClient.requestFrom(this->slaveId, INPUT_REGISTERS, 30000, 16)) {
        Serial.println(mbRtuClient.lastError());
      } else {
        irDataStorage[0] = mbRtuClient.read();

        irDataStorage[1] = (mbRtuClient.read() & (1 << 6)) == (1 << 6);  // on/off statue

        irDataStorage[2] = mbRtuClient.read() * 0;

        irDataStorage[3] = mbRtuClient.read();  // operation mode

        irDataStorage[4] = mbRtuClient.read();  // manual mode fan speed

        irDataStorage[5] = mbRtuClient.read() / 100;  // room temp

        irDataStorage[6] = mbRtuClient.read() / 100;  // set temp

        irDataStorage[7] = mbRtuClient.read() * 0;

        irDataStorage[8] = mbRtuClient.read() * 0;

        irDataStorage[9] = mbRtuClient.read() * 0;

        irDataStorage[10] = mbRtuClient.read() * 0;

        irDataStorage[11] = mbRtuClient.read();  // fan speed

        irDataStorage[12] = mbRtuClient.read() * 0;

        irDataStorage[13] = mbRtuClient.read() * 0;

        irDataStorage[14] = mbRtuClient.read() * 0;

        irDataStorage[15] = mbRtuClient.read() * 0;

        // for (size_t i = 0; i < 16; i++) {
        //   irDataStorage[i] = mbRtuClient.read();
        //   Serial.println(irDataStorage[i]);
        // }
      }

      if (!mbRtuClient.requestFrom(this->slaveId, HOLDING_REGISTERS, 40000, 13)) {
        Serial.println(mbRtuClient.lastError());
      } else {
        for (size_t i = 0; i < HR_SIZE; i++) {
          hrDataStorage[i] = mbRtuClient.read();
          Serial.println(hrDataStorage[i]);
        }
      }

      if (!mbRtuClient.requestFrom(this->slaveId, DISCRETE_INPUTS, 20000, 34)) {
        Serial.println(mbRtuClient.lastError());
      } else {
        for (size_t i = 0; i < DIR_SIZE; i++) {
          dirDataStorage[i] = mbRtuClient.read();
          Serial.println(dirDataStorage[i]);
        }
      }
    }
  }


  ~SubModbus() {}
};

extern SubModbus subModbus;

#endif