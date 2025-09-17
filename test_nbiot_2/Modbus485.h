#include "Globals.h"

#ifndef Modbus485_h
#define Modbus485_h

#define RE_DE_PIN 22

class Modbus485 {
public:
  Modbus485(){};

  void setup() {
    pinMode(RE_DE_PIN, OUTPUT);
    digitalWrite(RE_DE_PIN, LOW);  // HIGH = send, LOW = receive
    Serial3.begin(9600, SERIAL_8N1);
  };

  void loop() {
    this->listen();
  }

  void listen() {
    digitalWrite(RE_DE_PIN, LOW);
    while (Serial3.available()) {
      char c = Serial3.read();
      if (c != '\r' && c != '\n') {
        serialRecv485 += c;
      }
      if (c == '\r') {
        this->answer();
      }
    }
  }

  void answer() {
    int idx = serialRecv485.indexOf("AT");
    if (idx > -1) {
      digitalWrite(RE_DE_PIN, HIGH);
      Serial3.println("[Hello from RGT Hello from RGT Hello from RGT]");
      Serial3.flush();
    }
  }

  ~Modbus485(){};
};

extern Modbus485 modbus485;

#endif
