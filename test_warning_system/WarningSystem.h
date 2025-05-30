#include "Utils.h"
#include "SystemEnums.h"

#ifndef warning_system_h
#define warning_system_h

#define _CODE_COUNT 10
#define _CODE_HSWW 0
#define _CODE_CSWW 1

class WarningSystem {
private:
  SystemStatus currentStatus = DEFAULT;

  unsigned long prevMillis = 0;
  unsigned long prevHeartBeatMillis = 0;

  byte recvStatus[_CODE_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // 0=DEFAULT, 1=ISSUE, 2=CANCEL
  byte prevStatus[_CODE_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // 0=IDLE_DEFAULT, 1=IDLE_BY_ISSUE, 2=IDLE_BY_CANCEL
  byte issuePins[_CODE_COUNT] = { 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  byte issueDuration[_CODE_COUNT] = { 30, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  byte cancelPins[_CODE_COUNT] = { 11, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  byte cancelDuration[_CODE_COUNT] = { 30, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

public:

  void print() {
    for (size_t i = 0; i < _CODE_COUNT; i++) {
      Serial.print("recvStatus: ");
      Serial.println(this->recvStatus[i]);
      Serial.print("prevStatus: ");
      Serial.println(this->prevStatus[i]);
      Serial.println("");
    }
  }

  void execAlarm(byte pin, byte duration) {
    digitalWrite(pin, HIGH);
    delay(duration);
    digitalWrite(pin, LOW);
  }

  void listen() {
    for (size_t i = 0; i < _CODE_COUNT; i++) {
      // DEFAULE/CANCEL => ISSUE
      if (
        (this->prevStatus[i] == 0 || this->prevStatus[i] == 2)
        && this->recvStatus[i] == 1) {
        // 1. activate alarm
        this->execAlarm(issuePins[i], issueDuration[i]);
        // 2. update status
        prevStatus[i] = 1;
      }

      // DEFAULE/ISSUE => CANCEL
      if (
        (this->prevStatus[i] == 0 || this->prevStatus[i] == 1)
        && this->recvStatus[i] == 2) {
        // 1. activate alarm
        this->execAlarm(cancelPins[i], cancelDuration[i]);
        // 2. update status
        prevStatus[i] = 2;
      }
    }
  }

  void setRecvStatus(byte i, byte status) {
    this->recvStatus[i] = status;
  }

  ~WarningSystem(){};
};

extern WarningSystem warningSystem;

#endif