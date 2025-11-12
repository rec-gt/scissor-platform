#ifndef SubRS485_H
#define SubRS485_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class SubRS485 {

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

  void printlnFlush(const String& cmd) {
    this->prepareSend();
    RS485Serial.println(cmd);
    RS485Serial.flush();
  }

public:
  SubRS485(void) {}

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

      if (c != '\r' && c != '\n') {
        rs485SerialRecv += c;
      }
      if (c == '\r') {
        this->answer();
        this->clear();
      }
    }
  }

  void answer() {
    cmpStr = F("AT");
    int idx = rs485SerialRecv.indexOf(cmpStr);

    if (idx > -1) {
      this->printlnFlush(F("[Hello from RGT]"));
    }
  }

  ~SubRS485() {}
};

extern SubRS485 rStd485;

#endif