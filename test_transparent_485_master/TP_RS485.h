#include "Arduino.h"

#ifndef TP_RS485_h
#define TP_RS485_h

#define TP_RS485_Serial Serial1

class TP_RS485 {
private:
  byte pin;  // DE & DR pin
  String lastSendStr = "";
  String receivedStr = "";

public:
  TP_RS485(byte pin)
    : pin(pin) {
    TP_RS485_Serial.begin(9600);
    pinMode(this->pin, OUTPUT);
  }

  void mode(bool m) {  // m == true => sender-mode (HIGH) | m == false => receiver-mode (LOW)
    digitalWrite(this->pin, m ? HIGH : LOW);
  }

  void sendMsg(String sendStr) {
    if (this->lastSendStr != sendStr) {
      TP_RS485_Serial.print(sendStr);
      this->lastSendStr == sendStr;

      Serial.print("Send, ");
      Serial.println(sendStr);
    }
  }

  void waitForMsg() {
    this->receivedStr = "";
    while (TP_RS485_Serial.available()) {
      receivedStr += (char)TP_RS485_Serial.read();
    }
  }

  String getMsg() {
    return this->receivedStr;
  }

  ~TP_RS485() {}
};

extern TP_RS485;

#endif
