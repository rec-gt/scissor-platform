#ifndef RStd485_H
#define RStd485_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class RStd485 {
public:
  RStd485(void) {}

  void init() {
    pinMode(RS485_RE_DE_PIN, OUTPUT);
    digitalWrite(RS485_RE_DE_PIN, LOW);  // HIGH = send, LOW = receive
    RS485Serial.begin(9600, SERIAL_8N1);
  }

  void loop() {
    this->listen();
  }

  void listen() {
    digitalWrite(RS485_RE_DE_PIN, LOW);
    delay(1);
    while (RS485Serial.available()) {
      char c = RS485Serial.read();

      Serial.print(c);

      if (c != '\r' && c != '\n') {
        rs485SerialRecv += c;
      }
      if (c == '\r') {
        this->answer();
      }
    }
  }

  void answer() {
    cmpStr = F("AT");
    int idx = rs485SerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      digitalWrite(RS485_RE_DE_PIN, HIGH);
      delay(1);
      RS485Serial.println(F("[Hello from RGT Hello from RGT Hello from RGT]"));
      RS485Serial.flush();
    }
  }

  ~RStd485() {}
};

extern RStd485 rStd485;

#endif