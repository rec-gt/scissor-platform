#ifndef DigitalOutput_H
#define DigitalOutput_H

class DigitalOutput {
private:
  byte pin;
  bool prevState = false;  // true = connect, false = cut

public:
  DigitalOutput(void) {}

  DigitalOutput(byte pin)
    : pin(pin) {
    pinMode(pin, OUTPUT);
    this->cut();
  }

  void control(bool state) {
    if (state != this->prevState) {
      digitalWrite(this->pin, state ? HIGH : LOW);
      this->prevState = state;
    }
  }

  void connect() {
    this->control(true);
  }

  void cut() {
    this->control(false);
  }

  bool getState() {
    return this->prevState;
  }

  ~DigitalOutput() {}
};

#endif