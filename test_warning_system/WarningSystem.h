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

  byte recvStatus[_CODE_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // 0=DEFAULT, 1=ISSUE, 2=CANCEL
  byte prevStatus[_CODE_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // 0=IDLE_DEFAULT, 1=IDLE_BY_ISSUE, 2=IDLE_BY_CANCEL

public:
  void execAlarmTasks() {
    for (size_t i = 0; i < _CODE_COUNT; i++) {
      if (
        (this->prevStatus[i] == 0 || this->prevStatus[i] == 2)
        && this->recvStatus[i] == 1) {
        // 1. activate alarm
        // 2. update status
        prevStatus[i] = 1;
      }
    }
  }

  bool is() {
    // return receivedStatus == currentStatus;
  }

  void setIssuedWarning(byte item) {
    // this->issuedWarning |= 1 << item;
  }

  void listen() {
    // if ((millis() - this->prevMillis) > (30UL * 1000UL)) {
    //   this->isActived = false;
    //   this->prevMillis = millis();
    // }
    // Serial.println("IsActived: " + String(this->isActived));
    // digitalWrite(10, this->isActived ? HIGH : LOW);
    // if (this->issuedWarning & (1 << WS_S_HSWW)) {
    // }
  }

  ~WarningSystem(){};
};

extern WarningSystem warningSystem;

#endif