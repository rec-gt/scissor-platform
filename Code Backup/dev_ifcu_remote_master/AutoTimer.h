#ifndef AutoTimer_h
#define AutoTimer_h

class AutoTimer {
private:
  unsigned long prevMillis = 0;
  bool _lock = false;

public:
  AutoTimer(){};

  bool autoExpire(unsigned long autoTimeout) {
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

  ~AutoTimer() {}
};

#endif