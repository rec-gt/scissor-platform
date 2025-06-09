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

  void on(byte i) {
    this->off();
    digitalWrite(this->speakerPin, HIGH);
    digitalWrite(this->audioPins[i], LOW);
  };

  void off() {
    digitalWrite(this->speakerPin, LOW);

    for (size_t i = 0; i < 8; i++) {
      digitalWrite(this->audioPins[i], HIGH);
    }

    delay(300);
  };

  ~Speaker() {}
};

extern Speaker speaker;

#endif