class AsyncTimer {
private:
  unsigned long timeout = 3000;
  unsigned long prevMillis = 0;

public:
  AsyncTimer(){};

  AsyncTimer(unsigned long timeout)
    : timeout(timeout){};

  bool isExpired(void) {
    return millis() - this->prevMillis > this->timeout;
  }

  bool refresh(void) {
    this->prevMillis = millis();
  }

  bool feed(void) {
    this->prevMillis = millis();
  }

  ~AsyncTimer() {}
};