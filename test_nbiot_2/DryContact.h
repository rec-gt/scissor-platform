#ifndef DryContact_H
#define DryContact_H

class DryContact {
private:
  byte pin;
  bool prevState = false;  // true = connect, false = cut

public:
  DryContact() {}

  DryContact(byte pin)
    : pin(pin) {
    pinMode(pin, INPUT_PULLUP);
  }

  bool isConnected() {
    return false;
  }
};

#endif