#ifndef SoftWatchdog_h
#define SoftWatchdog_h

class SoftWatchdog {
private:
  typedef void (*CallbackFunction)(void);

  uint32_t timeout = 60UL * 1000UL;
  uint32_t prevMillis = 0;
  bool isEnabled = false;
  CallbackFunction callback = nullptr;

public:
  SoftWatchdog() {}

  void enable(uint32_t timeout) {
    this->timeout = timeout;
    this->isEnabled = true;
  }

  void setCallback(void (*callback)()) {
    this->callback = callback;
  }

  void loop(void) {
    if (this->isEnabled) {
      if ((millis() - this->prevMillis) > this->timeout) {
        this->reboot();
      }
    }
  }

  void reset(void) {  // feed
    this->prevMillis = millis();
  };

  void reboot(void) {
    if (this->callback != nullptr) {
      this->callback();
    }
    this->reset();
  }

  ~SoftWatchdog(){};
};

#endif
