#include "Speaker.h"
#include "AsyncTimer.h"
#include "Enums.h"
#include <EEPROM.h>

#ifndef AlarmSystem_h
#define AlarmSystem_h

#define EEPROM_LAST_STATUS_ADDR 1

AsyncTimer replayTimer(2UL * 60UL * 1000UL);

class AlarmSystem {
private:
  byte speakerIdx = 0;
  byte accumulate = 0;
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

  void replay() {
    if (this->recvStatus == this->prevStatus) {
      // state unchange, count for replay
      if (this->recvStatus != SYS_INIT && this->recvStatus != SYS_CANCEL_OUTDOOR && this->recvStatus != SYS_CANCEL_INDOOR) {
        // count for replay
        if (replayTimer.isExpired()) {
          speaker.on(this->speakerIdx);
          replayTimer.refresh();
        }
      }
    } else {
      // state change, force issue/cancel
      if (this->recvStatus != SYS_INIT && this->recvStatus != SYS_CANCEL_OUTDOOR && this->recvStatus != SYS_CANCEL_INDOOR) {
        this->update();
        speaker.on(this->speakerIdx);
      }
    }
  }

  ~AlarmSystem(){};
};

extern AlarmSystem alarmSystem;

#endif