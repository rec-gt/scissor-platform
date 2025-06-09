#include "Speaker.h"
#include "Enums.h"

#ifndef AlarmSystem_h
#define AlarmSystem_h

class AlarmSystem {
private:
  SystemStatus prevStatus = SYS_INIT;
  SystemStatus recvStatus = SYS_INIT;

public:
  AlarmSystem(){};

  void set(String payload) {
    int idx = payload.indexOf("1");
    if (idx > -1) {

      this->recvStatus = idx + 1;
    }
  };

  void update() {
    this->prevStatus = this->recvStatus;
  };

  void listen(void) {
    if (this->recvStatus != SYS_INIT && this->recvStatus != SYS_CANCEL_OUTDOOR && this->recvStatus != SYS_CANCEL_INDOOR) {
      if (this->recvStatus != this->prevStatus) {
        this->update();
        speaker.on(this->recvStatus);
      }
    }

    if (this->recvStatus == SYS_CANCEL_OUTDOOR) {
      if (this->recvStatus != this->prevStatus) {
        this->update();
        this->alarmCancel();
      }
    }

    if (this->recvStatus == SYS_CANCEL_INDOOR) {
      if (this->recvStatus != this->prevStatus) {
        this->update();
        this->alarmCancel();
      }
    }
  };

  void alarmActive(void) {
    speaker.on(1);
  }

  void alarmCancel(void) {
    speaker.on(7);
  }

  ~AlarmSystem(){};
};

extern AlarmSystem alarmSystem;

#endif