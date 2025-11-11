#include "./Globals.h"

#ifndef Modbus485_h
#define Modbus485_h

class Modbus485 {
protected:
  byte RE_DE_PIN = 22;
public:
  Modbus485() {
    pinMode(this->RE_DE_PIN, OUTPUT);
    digitalWrite(this->RE_DE_PIN, LOW);  // HIGH = send, LOW = receive
    Serial3.begin(9600, SERIAL_8N1);
  };

  ~Modbus485(){};
};

extern Modbus485 modbus485;

#endif
