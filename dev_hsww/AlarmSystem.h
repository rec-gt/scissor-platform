#include "Speaker.h"
#include "AsyncTimer.h"
#include "Enums.h"
#include <EEPROM.h>

#ifndef AlarmSystem_h
#define AlarmSystem_h

#define EEPROM_LAST_STATUS_ADDR 1

AsyncTimer replayTimer(1UL * 60UL * 1000UL);

class AlarmSystem {
private:
  SystemStatus prevStatus = SYS_INIT;
  SystemStatus recvStatus = SYS_INIT;

public:
  AlarmSystem(){};

  void init(SystemStatus initStatus) {
    this->prevStatus = initStatus;
  };

  void set(String payload) {
    int idx = payload.indexOf("1");
    if (idx > -1) {
      this->recvStatus = idx + 1;
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
        speaker.on(this->recvStatus);
      }
    }

    if (this->recvStatus == SYS_CANCEL_OUTDOOR || this->recvStatus == SYS_CANCEL_INDOOR) {
      if (this->recvStatus != this->prevStatus) {
        this->update();
        speaker.on(this->recvStatus);
      }
    }
  };

  void replay() {
    if (this->recvStatus == this->prevStatus) {
      // state unchange, count for replay
      if (this->recvStatus != SYS_INIT && this->recvStatus != SYS_CANCEL_OUTDOOR && this->recvStatus != SYS_CANCEL_INDOOR) {
        // count for replay
        if (replayTimer.isExpired()) {
          speaker.on(this->recvStatus);
          replayTimer.refresh();
        }
      }
    } else {
      // state change, force issue/cancel
      if (this->recvStatus != SYS_INIT && this->recvStatus != SYS_CANCEL_OUTDOOR && this->recvStatus != SYS_CANCEL_INDOOR) {
        this->update();
        speaker.on(this->recvStatus);
      }
    }
  }

  ~AlarmSystem(){};
};

extern AlarmSystem alarmSystem;

#endif