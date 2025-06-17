#include "Enums.h"

#ifndef Speaker_h
#define Speaker_h

class Speaker {
private:
  byte speakerPin = 31;
  byte audioPins[8] = { 20, 21, 22, 23, 24, 25, 26, 27 };

  void init(void) {
    pinMode(this->speakerPin, OUTPUT);
    digitalWrite(this->speakerPin, HIGH);

    for (size_t i = 0; i < 8; i++) {
      pinMode(this->audioPins[i], OUTPUT);
      digitalWrite(this->audioPins[i], HIGH);
    }
  }

public:
  Speaker() {
    this->init();
  };

  void on(SystemStatus status) {
    delay(300);
    this->off();
    delay(300);

    byte idx = status - 1;
    
    digitalWrite(this->speakerPin, HIGH);
    delay(300);
    digitalWrite(this->audioPins[idx], LOW);
    delay(300);
  };

  void off() {
    digitalWrite(this->speakerPin, LOW);

    for (size_t i = 0; i < 8; i++) {
      digitalWrite(this->audioPins[i], HIGH);
    }
  };

  ~Speaker() {}
};

extern Speaker speaker;

#endif