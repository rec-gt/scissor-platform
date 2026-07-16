#ifndef DigitalInput_H
#define DigitalInput_H

class DigitalInput {
private:
  byte pin;
  byte state = LOW;
  uint32_t prevMillis;
  bool stateChanged = false;

public:
  DigitalInput() {}

  DigitalInput(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    byte reading = digitalRead(this->pin);

    if (reading != this->state) {
      if ((millis() - this->prevMillis) > 50) {
        this->stateChanged = true;
        this->state = reading;
      }
    } else {
      this->prevMillis = millis();
    }
  }

  byte getState() {
    return !this->state;
  }

  bool hasStateChange() {  // call了才會reset
    bool res = this->stateChanged;
    this->stateChanged = false;
    return res;
  }
};

#endif