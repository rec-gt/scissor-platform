#ifndef Timer_H
#define Timer_H

class Timer {
private:
  uint16_t timeout = 3000;
  uint16_t prevMillis = 0;
  bool _lock = false;

public:
  Timer(){};

  Timer(uint16_t timeout)
    : timeout(timeout){};

  bool isExpired(void) {
    return millis() - this->prevMillis > this->timeout;
  }

  void refresh(void) {
    this->prevMillis = millis();
  }

  bool autoTimeout(uint16_t _to) {
    bool isTimeout = millis() - this->prevMillis > _to;

    if (this->_lock == false) {
      this->prevMillis = millis();
      this->_lock = true;
    }

    if (isTimeout) {
      this->_lock = false;
    }

    isTimeout = millis() - this->prevMillis > _to;

    return isTimeout;
  }


  ~Timer() {}
};

#endif