#ifndef SubRS485_H
#define SubRS485_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class SubRS485 {

private:
  void prepareRecv() {
    digitalWrite(RS485_RE_DE_PIN, LOW);  // HIGH = send, LOW = receive
    delay(2);
  }

  void prepareSend() {
    digitalWrite(RS485_RE_DE_PIN, HIGH);  // HIGH = send, LOW = receive
    delay(2);
  }

  void clear() {
    rs485SerialRecv = F("");
  }

  void printlnFlush(const String& cmd) {
    this->prepareSend();
    RS485Serial.println(cmd);
    RS485Serial.flush();
    delay(2);
  }

  uint16_t prevMillis = millis();

  byte mode = 0;  // 0 = ask-reply mode, 1 = active-send mode

public:
  SubRS485(void) {}

  void init() {
    pinMode(RS485_RE_DE_PIN, OUTPUT);
    this->prepareRecv();
    RS485Serial.begin(9600, SERIAL_8N1);
  }

  void loop() {
    this->listen();

    if (this->mode == 0) {

    } else if (this->mode == 1) {
      this->say();
    }
  }

  void listen() {
    this->prepareRecv();
    while (RS485Serial.available()) {
      char c = RS485Serial.read();
      if (c != '\r' && c != '\n') {
        rs485SerialRecv += c;
      }
      if (c == '\r') {
        Serial.print(rs485SerialRecv);
        this->answer();
        this->clear();
      }
    }
  }

  void answer() {
    if (rs485SerialRecv == F("AT")) {
      this->printlnFlush(F("[<<reply to computer, reply to computer, reply to computer]"));
    }

    if (rs485SerialRecv == F("AT+MODE=MANUAL")) {
      this->mode = 0;
    }

    if (rs485SerialRecv == F("AT+MODE=SEND")) {
      this->mode = 1;
    }
  }

  void say() {
    uint16_t currMillis = millis();
    if (currMillis - this->prevMillis > 1000) {
      this->printlnFlush(F("[>>send to computer, send to computer, send to computer]"));
      this->prevMillis = millis();
    }
  }

  ~SubRS485() {}
};

extern SubRS485 subRS485;
// 4:57
#endif