#ifndef Timer_H
#define Timer_H

class Timer {
private:
  unsigned long prevMillis = 0;
  bool _lock = false;

public:
  Timer(){};

  bool autoTimeout(unsigned long timeout) {
    bool isTimeout = millis() - this->prevMillis > timeout;

    if (this->_lock == false) {
      this->prevMillis = millis();
      this->_lock = true;
    }

    if (isTimeout) {
      this->_lock = false;
    }

    isTimeout = millis() - this->prevMillis > timeout;

    return isTimeout;
  }

  ~Timer() {}
};

#endif