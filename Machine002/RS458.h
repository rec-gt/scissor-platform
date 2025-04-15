#include "Arduino.h"
#ifndef RS485_h
#define RS485_h
#define RS485_Serial Serial2

class RS485 {
private:
  byte pin = 12;  // DE & DR pin, hardcoded here
  String lastSendStr = "";
  String receivedStr = "";
public:

  RS485() {
    RS485_Serial.begin(9600);
    pinMode(this->pin, OUTPUT);
    this->mode(HIGH);
  }

  void mode(bool m) {  // m == true => sender-mode (HIGH) | m == false => receiver-mode (LOW)
    digitalWrite(this->pin, m ? HIGH : LOW);
  }

  void waitForMsg() {
    this->receivedStr = "";
    while (RS485_Serial.available()) {
      receivedStr += (char)RS485_Serial.read();
    }
  }

  void sendMsg(String sendStr) {
    if (this->lastSendStr != sendStr) {
      RS485_Serial.print(sendStr);
      this->lastSendStr == sendStr;

      Serial.print("Send, ");
      Serial.println(sendStr);
    }
  }

  ~RS485() {}
};

extern RS485;

#endif
