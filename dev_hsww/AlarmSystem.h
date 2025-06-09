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

  void set(SystemStatus status) {
    this->recvStatus = status;
  };

  void update() {
    this->prevStatus = this->recvStatus;
  };

  void listen(void) {
    if (this->prevStatus == SYS_INIT) {
      if (this->recvStatus != this->prevStatus) {
        this->update();
      }
    }

    if (this->recvStatus == SYS_CANCEL) {
      if (this->recvStatus != this->prevStatus) {
        this->update();
        this->alarmCancel();
      }
    }
  };

  void alarmCancel(void) {
    speaker.on(7);
  }

  ~AlarmSystem(){};
};

extern AlarmSystem alarmSystem;

#endif