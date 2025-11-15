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
    delay(1);
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
    if (rs485SerialRecv == F("AT")) {
      this->printlnFlush(F("[Hello from REC-GT]"));
    }

    if (rs485SerialRecv == F("AT+ALARM=1")) {
      relay.connect();
      Serial.println(F("Alarm ON"));
    }

    if (rs485SerialRecv == F("AT+ALARM=0")) {
      relay.cut();
      Serial.println(F("Alarm OFF"));
    }

    int idx = rs485SerialRecv.indexOf(F("AT+STATUS="));
    if (idx > -1) {
      {
        Serial.println(rs485SerialRecv.substring(10, 10 + 20));
      }
    }
  }

  ~SubRS485() {}
};

extern SubRS485 rStd485;

#endif