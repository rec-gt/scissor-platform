#ifndef RStd485_H
#define RStd485_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class RStd485 {
public:
  RStd485(void) {}

  void init() {
    recdStd485.begin(9600);
  }

  void loop() {
    this->listen();
  }

  void listen() {
    while (recdStd485.available()) {
      char c = recdStd485.read();

      Serial.print(c);

      if (c != '\r' && c != '\n') {
        rs485SerialRecv += c;
      }
      if (c == '\r') {
        RS485.beginTransmission();
        RS485.println(rs485SerialRecv);
        RS485.endTransmission();
      }
    }
  }

  ~RStd485() {}
};

extern RStd485 rStd485;

#endif