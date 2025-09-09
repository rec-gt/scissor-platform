#ifndef DryContact_H
#define DryContact_H

class DryContact {
private:
  byte pin;

public:
  DryContact(byte pin)
    : pin(pin) {
    pinMode(this->pin, INPUT_PULLUP);
  }

  bool isConnected() {
    return digitalRead(this->pin) == LOW;
  }
};

#endif