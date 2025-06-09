#include "Speaker.h"
#include "Enums.h"

#ifndef System_h
#define System_h

class System {
private:
  SystemStatus prevStatus = SYS_INIT;
  SystemStatus recvStatus = SYS_INIT;

public:
  System(){};

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

  ~System(){};
};

extern System system;

#endif