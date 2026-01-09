#ifndef Counter_H
#define Counter_H

class Counter {
private:
  byte count = 0;

public:
  Counter(){};

  void accu(byte i = 1) {
    this->count += i;
  }

  bool over(byte b = 10) {
    bool res = (this->count >= b);

    if (res) {
      this->count = 0;
    }

    return res;
  }

  ~Counter() {}
};

#endif