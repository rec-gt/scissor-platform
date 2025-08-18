#ifndef DigitalInput_H
#define DigitalInput_H

class DigitalInput {
private:
  byte pin;
  byte state = LOW;
  unsigned long prevMillis;

public:
  DigitalInput(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  void listen() {
    byte reading = digitalRead(this->pin);

    if (reading != this->state) {
      if ((millis() - this->prevMillis) > 50) {
        this->state = reading;
      }
    } else {
      this->prevMillis = millis();
    }
  }

  byte getState() {
    return this->state;
  }
};

#endif