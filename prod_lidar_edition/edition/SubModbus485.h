#include "../core/Modbus485.h"

#ifndef SubModbus485_h
#define SubModbus485_h


class SubModbus485 : public Modbus485 {
public:
  void loop() {
    this->listen();
  }

  void listen() {
    digitalWrite(RE_DE_PIN, LOW);
    delay(1);
    while (Serial3.available()) {
      char c = Serial3.read();

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
      digitalWrite(RE_DE_PIN, HIGH);
      delay(2);
      Serial3.println(F("[Hello from SubModbus]"));
      Serial3.flush();
    }
  }
};

extern SubModbus485 Submodbus485;

#endif
