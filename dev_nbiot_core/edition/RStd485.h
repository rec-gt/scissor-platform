#ifndef RStd485_H
#define RStd485_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class RStd485 {

private:
  void prepareRecv() {
    digitalWrite(RS485_RE_DE_PIN, LOW);  // HIGH = send, LOW = receive
    delay(1);
  }

  void prepareSend() {
    digitalWrite(RS485_RE_DE_PIN, HIGH);  // HIGH = send, LOW = receive
    delay(1);
  }

  void clear() {
    rs485SerialRecv = F("");
  }

public:
  RStd485(void) {}

  void init() {
    pinMode(RS485_RE_DE_PIN, OUTPUT);
    this->prepareRecv();
    RS485Serial.begin(9600, SERIAL_8N1);
  }

  void loop() {
    this->listen();
  }

  void listen() {
    this->prepareRecv();
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
      this->prepareSend();
      RS485Serial.println(F("[Hello from RGT Hello from RGT Hello from RGT]"));
      RS485Serial.flush();
      this->clear();
    }
  }

  ~RStd485() {}
};

extern RStd485 rStd485;

#endif