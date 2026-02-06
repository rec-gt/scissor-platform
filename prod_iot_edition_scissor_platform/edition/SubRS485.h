#ifndef SubRS485_H
#define SubRS485_H

#include "../core/Globals.h"
#include "./SubGlobals.h"

class SubRS485 {

private:
  bool unlocked = false;

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
    this->unlocked = false;
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
    if (rs485SerialRecv == F("AT+UNLOCK=RGT@2011")) {
      this->unlocked = true;
      this->printlnFlush(F("MODULE UNLOCKED"));
    }

    if (this->unlocked) {
      if (rs485SerialRecv == F("AT+ALL=600")) {
        sensorThresholdDistance = 600;
        EEPROM.put(EEP_ADDR_THRESHOLD_DISTANCE, 6);
        this->printlnFlush(F("OK, THRESHOLD: 600"));
      }

      else if (rs485SerialRecv == F("AT+ALL=800")) {
        sensorThresholdDistance = 800;
        EEPROM.put(EEP_ADDR_THRESHOLD_DISTANCE, 8);
        this->printlnFlush(F("OK, THRESHOLD: 800"));
      }

      else if (rs485SerialRecv == F("AT+ALL=1000")) {
        sensorThresholdDistance = 1000;
        EEPROM.put(EEP_ADDR_THRESHOLD_DISTANCE, 10);
        this->printlnFlush(F("OK, THRESHOLD: 1000"));
      }

      else if (rs485SerialRecv == F("AT+ALL=1200")) {
        sensorThresholdDistance = 1200;
        EEPROM.put(EEP_ADDR_THRESHOLD_DISTANCE, 12);
        this->printlnFlush(F("OK, THRESHOLD: 1200"));
      }

      else if (rs485SerialRecv == F("AT+ESCAPE=5")) {
        escapeCountDown = 5;
        EEPROM.put(EEP_ADDR_ESCAPE_COUNT_DOWN, 5);
        this->printlnFlush(F("OK, ESCAPE COUNTDOWN: 5s"));
      }

      else if (rs485SerialRecv == F("AT+ESCAPE=10")) {
        escapeCountDown = 10;
        EEPROM.put(EEP_ADDR_ESCAPE_COUNT_DOWN, 10);
        this->printlnFlush(F("OK, ESCAPE COUNTDOWN: 10s"));
      }

      else if (rs485SerialRecv == F("AT+ESCAPE=15")) {
        escapeCountDown = 15;
        EEPROM.put(EEP_ADDR_ESCAPE_COUNT_DOWN, 15);
        this->printlnFlush(F("OK, ESCAPE COUNTDOWN: 15s"));
      }

      else if (rs485SerialRecv == F("AT+LOCK")) {
        this->unlocked = false;
        this->printlnFlush(F("OK, MODULE LOCKED"));
      }
    }
  }

  ~SubRS485() {}
};

extern SubRS485 subRS485;
// 4:57
#endif