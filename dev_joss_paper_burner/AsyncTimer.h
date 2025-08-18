#ifndef ASYNCTIMER_H
#define ASYNCTIMER_H

class AsyncTimer {
private:
  unsigned long timeout = 3000;
  unsigned long prevMillis = 0;
  bool _lock = false;

public:
  AsyncTimer(){};

  AsyncTimer(unsigned long timeout)
    : timeout(timeout){};

  bool isExpired(void) {
    return millis() - this->prevMillis > this->timeout;
  }

  void refresh(void) {
    this->prevMillis = millis();
  }

  bool autoExpired(unsigned long autoTimeout) {
    bool isTimeout = millis() - this->prevMillis > autoTimeout;

    if (this->_lock == false) {
      this->prevMillis = millis();
      this->_lock = true;
    }

    if (isTimeout) {
      this->_lock = false;
    }

    isTimeout = millis() - this->prevMillis > autoTimeout;

    return isTimeout;
  }

  ~AsyncTimer() {}
};

#endif