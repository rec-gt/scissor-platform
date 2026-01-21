#ifndef Test_H
#define Test_H

#include "../core/Timer.h"
#include "../core/Globals.h"

Timer timerDO;
Timer timerAO;

class Test {
private:
  unsigned long prevMillis = millis();

public:
  Test(){};

  int doCnt = 0;
  bool doToggle = true;

  void DO() {
    if (timerDO.autoTimeout(2000)) {
      this->doToggle ? digitalOutputs[this->doCnt].connect() : digitalOutputs[this->doCnt].cut();
      if (this->doCnt++ == DO_NUMS - 1) {
        this->doCnt = 0;
        this->doToggle = !this->doToggle;
      }
    }
  }

  byte aoStrength = 0;  // max 255
  void AO() {
    if (timerAO.autoTimeout(520)) {
      for (size_t i = 0; i < AO_NUMS; i++) {
        analogOutputs[i].set(this->aoStrength);
      }
      this->aoStrength += 5;
    }
  }

  ~Test(){};
};

#endif