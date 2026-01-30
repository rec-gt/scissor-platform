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
        break;
      }
    }

    this->answer();
    this->clear();
  }

  void answer() {
    if (rs485SerialRecv == F("AT+ALL=600")) {
      this->printlnFlush(F("[ok]"));
    }

    if (rs485SerialRecv == F("AT+ALL=800")) {
      this->printlnFlush(F("[ok]"));
    }

    if (rs485SerialRecv == F("AT+ALL=1000")) {
      this->printlnFlush(F("[ok]"));
    }

    if (rs485SerialRecv == F("AT+ALL=1200")) {
      this->printlnFlush(F("[ok]"));
    }

    if (rs485SerialRecv == F("AT+FB=600")) {
      this->printlnFlush(F("[ok]"));
    }

    if (rs485SerialRecv == F("AT+FB=800")) {
      this->printlnFlush(F("[ok]"));
    }

    if (rs485SerialRecv == F("AT+FB=1000")) {
      this->printlnFlush(F("[ok]"));
    }

    if (rs485SerialRecv == F("AT+FB=1200")) {
      this->printlnFlush(F("[ok]"));
    }
  }

  ~SubRS485() {}
};

extern SubRS485 subRS485;
// 4:57
#endif