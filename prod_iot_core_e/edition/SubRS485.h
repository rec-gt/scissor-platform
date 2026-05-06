#include "Arduino.h"
#ifndef SubRS485_H
#define SubRS485_H

#include "../core/Globals.h"
#include "./SubGlobals.h"
#include "./Memory.h"

class SubRS485 {

private:
  void prepareRecv() {
    digitalWrite(RS485_RE_DE_PIN, LOW);  // HIGH = send, LOW = receive
  }

  void prepareSend() {
    digitalWrite(RS485_RE_DE_PIN, HIGH);  // HIGH = send, LOW = receive
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
    RS485Serial.begin(115200, SERIAL_8N1);
    rs485Lock.lock();
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
    if (rs485Lock.isReleased()) {
      /* === Module Unlocked === */
      if (rs485SerialRecv == F("AT")) {
        this->printlnFlush(F("AT OK"));
        String target = "";
        memory.readStrRange(0, 32, target);
        Serial.println(target);
        memory.readStrRange(32, 37, target);
        Serial.println(target);
      }

      /* === Config Host === */
      if (rs485SerialRecv.indexOf(F("AT+HOST=")) > -1) {
        memory.writeStr(0, 32, rs485SerialRecv.substring(8, 8 + 32));
        String target = "";
        memory.readStr(0, 32, target);
        Serial.println(target);
      }

      /* === Config Port === */
      if (rs485SerialRecv.indexOf(F("AT+PORT=")) > -1) {
        memory.writeStr(32, 8, rs485SerialRecv.substring(8, 8 + 8));
        String target = "";
        memory.readStr(32, 8, target);
        Serial.println(target);
      }

      /* === Config Username === */
      if (rs485SerialRecv.indexOf(F("AT+USERNAME=")) > -1) {
        memory.writeStr(40, 16, rs485SerialRecv.substring(12, 12 + 16));
        String target = "";
        memory.readStr(40, 16, target);
        Serial.println(target);
      }

      /* === Config Password === */
      if (rs485SerialRecv.indexOf(F("AT+PASSWORD=")) > -1) {
        memory.writeStr(56, 16, rs485SerialRecv.substring(12, 12 + 16));
        String target = "";
        memory.readStr(56, 16, target);
        Serial.println(target);
      }

      /* === Lock & Auto Lock === */
      if (rs485SerialRecv == F("AT+LOCK")) {
        rs485Lock.lock();
        this->printlnFlush(F("OK, MODULE LOCKED"));
      }

      if (rs485LockTimer.autoTimeout(30000)) {
        rs485Lock.lock();
        this->printlnFlush(F("TIMEOUT, MODULE AUTO LOCKED"));
      }
    } else {
      /* === Module Locked === */
      if (rs485SerialRecv == F("AT+UNLOCK=RGT@2011")) {
        rs485Lock.release();
        rs485LockTimer.refresh();
        this->printlnFlush(F("MODULE UNLOCKED"));
      }
    }
  }

  ~SubRS485() {}
};

extern SubRS485 subRS485;
// 4:57
#endif