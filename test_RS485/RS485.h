#include "Arduino.h"

#ifndef RS485_Send_h
#define RS485_Send_h

#define RS485_Serial Serial1

class RS485_Send {
private:
  byte pin_DE;
  byte pin_RE;
  String lastSendStr = "";
  String receivedStr = "";

public:
  RS485(byte pin_DE, byte pin_RE)
    : pin_DE(pin_DE), pin_RE(pin_RE) {
    RS485_Serial.begin(9600);
    pinMode(this->pin_DE, OUTPUT);
    pinMode(this->pin_RE, OUTPUT);
  }

  void mode(bool m) {  // m == true => sender-mode (HIGH) | m == false => receiver-mode (LOW)
    digitalWrite(this->pin_DE, m ? HIGH : LOW);
    digitalWrite(this->pin_RE, m ? HIGH : LOW);
  }

  void sendMsg(String sendStr) {
    if (this->lastSendStr != sendStr) {
      RS485_Serial.print(sendStr);
      this->lastSendStr == sendStr;

      Serial.print("Send, ");
      Serial.println(sendStr);
    }
  }

  void sendWrite(byte msg[], size_t msg_size) {
    RS485_Serial.write(msg, msg_size);
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

  void waitReceived() {
    while (RS485_Serial.available()) {
      Serial.print(RS485_Serial.read());
    }
    Serial.println();
  }

  ~RS485() {}
};

extern RS485;

#endif
