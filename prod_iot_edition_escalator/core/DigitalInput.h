#ifndef DigitalInput_H
#define DigitalInput_H

class DigitalInput {
private:
  byte pin;
  byte state = LOW;
  byte prevState = LOW;
  uint32_t prevMillis;

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
        this->prevState = this->state;
        this->state = reading;
      }
    } else {
      this->prevMillis = millis();
    }
  }

  byte getState() {
    return !this->state;
  }

  byte getPrevState() {
    return !this->prevState;
  }
};

#endif