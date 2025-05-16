#include "Utils.h"

#ifndef warning_system_h
#define warning_system_h

class WarningSystem {
private:
  bool isActived = false;
  unsigned long prevMillis;

public:
  void setIsActived(bool b) {
    this->isActived = b;
    this->prevMillis = millis();
  }

  void listen() {
    if (millis() - this->prevMillis > 5 * 60 * 1000) {
      this->isActived = false;
      this->prevMillis = millis();
    }
  }

  ~WarningSystem(){};
};

extern WarningSystem warningSystem;

#endif