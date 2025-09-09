#include "Arduino.h"

#ifndef RS485_h
#define RS485_h

#define RS485_Serial Serial2

class RS485 {
private:
  byte pin;  // DE & DR pin
  String lastSendStr = "";
  String receivedStr = "";

public:
  RS485(byte pin)
    : pin(pin) {
    RS485_Serial.begin(9600);
    pinMode(this->pin, OUTPUT);
  }

  void mode(bool m) {  // m == true => sender-mode (HIGH) | m == false => receiver-mode (LOW)
    digitalWrite(this->pin, m ? HIGH : LOW);
  }

  void sendMsg(String sendStr) {
    if (this->lastSendStr != sendStr) {
      RS485_Serial.print(sendStr);
      this->lastSendStr == sendStr;

      Serial.print("Send, ");
      Serial.println(sendStr);
    }
  }

  void waitForMsg() {
    this->receivedStr = "";
    while (RS485_Serial.available()) {
      receivedStr += (char)RS485_Serial.read();
    }
  }

  String getMsg() {
    return this->receivedStr;
  }

  ~RS485() {}
};

extern RS485;

#endif
