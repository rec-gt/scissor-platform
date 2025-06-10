#include "Speaker.h"
#include "Enums.h"
#include <EEPROM.h>

#ifndef AlarmSystem_h
#define AlarmSystem_h

#define EEPROM_LAST_STATUS_ADDR 1

class AlarmSystem {
private:
  byte speakerIdx = 0;
  SystemStatus prevStatus = SYS_INIT;
  SystemStatus recvStatus = SYS_INIT;

public:
  AlarmSystem(){};

  void set(String payload) {
    int idx = payload.indexOf("1");
    if (idx > -1) {
      this->recvStatus = idx + 1;
      this->speakerIdx = idx;
    }
  };

  void update() {
    this->prevStatus = this->recvStatus;
    EEPROM.put(EEPROM_LAST_STATUS_ADDR, this->prevStatus);
  };

  void listen(void) {
    if (this->recvStatus != SYS_INIT && this->recvStatus != SYS_CANCEL_OUTDOOR && this->recvStatus != SYS_CANCEL_INDOOR) {
      if (this->recvStatus != this->prevStatus) {
        this->update();
        speaker.on(this->speakerIdx);
      }
    }

    if (this->recvStatus == SYS_CANCEL_OUTDOOR || this->recvStatus == SYS_CANCEL_INDOOR) {
      if (this->recvStatus != this->prevStatus) {
        this->update();
        speaker.on(this->speakerIdx);
      }
    }
  };

  ~AlarmSystem(){};
};

extern AlarmSystem alarmSystem;

#endif