#include "Arduino.h"

#ifndef RS485Module_h
#define RS485Module_h
#define RS485Module_Serial Serial2

class RS485Module {
private:
  byte pin = 12;  // DE & DR pin, hardcoded here
  String lastSendStr = "";
  String receivedStr = "";

public:
  RS485Module() {
    RS485Module_Serial.begin(9600);
    pinMode(this->pin, OUTPUT);
    this->mode(LOW);
  }

  void mode(bool m) {  // m == true => sender-mode (HIGH) | m == false => receiver-mode (LOW)
    digitalWrite(this->pin, m ? HIGH : LOW);
    digitalWrite(12, LOW);
  }

  void waitForMsg() {
    this->receivedStr = "";
    while (RS485Module_Serial.available()) {
      receivedStr += (char)RS485Module_Serial.read();
    }
  }

  void sendMsg(String sendStr) {
    if (this->lastSendStr != sendStr) {
      RS485Module_Serial.print(sendStr);
      this->lastSendStr == sendStr;

      Serial.print("Send, ");
      Serial.println(sendStr);
    }
  }

  ~RS485Module(){};
};

extern RS485Module rs485module;

#endif
