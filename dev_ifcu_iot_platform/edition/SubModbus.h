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
        dataStorage[0] = mbRtuClient.read();

        dataStorage[1] = (mbRtuClient.read() & (1 << 6));  // on/off statue

        dataStorage[2] = mbRtuClient.read() * 0;

        dataStorage[3] = mbRtuClient.read();  // operation mode

        dataStorage[4] = mbRtuClient.read();  // manual mode fan speed

        dataStorage[5] = mbRtuClient.read();  // room temp

        dataStorage[6] = mbRtuClient.read();  // set temp

        dataStorage[7] = mbRtuClient.read() * 0;

        dataStorage[8] = mbRtuClient.read() * 0;

        dataStorage[9] = mbRtuClient.read() * 0;

        dataStorage[10] = mbRtuClient.read() * 0;

        dataStorage[11] = mbRtuClient.read();  // fan speed

        dataStorage[12] = mbRtuClient.read() * 0;

        dataStorage[13] = mbRtuClient.read() * 0;

        dataStorage[14] = mbRtuClient.read() * 0;

        dataStorage[15] = mbRtuClient.read() * 0;

        // for (size_t i = 0; i < 16; i++) {
        //   dataStorage[i] = mbRtuClient.read();
        //   Serial.println(dataStorage[i]);
        // }
      }
    }
  }


  ~SubModbus() {}
};

extern SubModbus subModbus;

#endif