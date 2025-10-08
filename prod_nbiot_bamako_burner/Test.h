#include "AsyncTimer.h"
#ifndef Test_H
#define Test_H

AsyncTimer timerDO;
AsyncTimer timerAO;

class Test {
private:
  unsigned long prevMillis = millis();

public:
  Test(){};

  int doCnt = 0;
  bool doToggle = true;

  void DO(DigitalOutput* dos) {
    if (timerDO.autoExpired(2000)) {
      this->doToggle ? dos[this->doCnt].connect() : dos[this->doCnt].cut();
      if (this->doCnt++ == DO_NUMS - 1) {
        this->doCnt = 0;
        this->doToggle = !this->doToggle;
      }
    }
  }

  byte aoStrength = 0;  // max 255
  void AO(AnalogOutput* aos) {
    if (timerAO.autoExpired(520)) {
      for (size_t i = 0; i < AO_NUMS; i++) {
        aos[i].set(this->aoStrength);
      }
      this->aoStrength += 5;
    }
  }

  ~Test(){};
};

#endif