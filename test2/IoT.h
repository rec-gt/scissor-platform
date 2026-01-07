#ifndef IOT_H
#define IOT_H

#include "Globals.h"

class IoT {
private:
  void clearSerialBuffer() {
    while (SerialIoT.read() > 0) { delay(1); };
  }

  void clearRecvBuffer() {
    iotSerialRecv = F("");
  }

  void listenSerial() {
    while (SerialIoT.available() > 0) {
      char c = SerialIoT.read();
      // Serial.print(c);

      if (c != '\r' && c != '\n' && c != ' ') {
        iotSerialRecv += c;
      }
    }
  }

  void handleResponse() {
    this->printRecv();
    this->clearRecvBuffer();
  }


public:
  void init() {
    SerialIoT.begin(115200);
    pinMode(24, OUTPUT);
    digitalWrite(24, HIGH);
  }

  void loop() {
    this->listenSerial();
    this->handleResponse();
  }

  void printlnFlush(const String& cmd) {
    this->clearSerialBuffer();  // 清空Serial
    SerialIoT.println(cmd);
    SerialIoT.flush();
    delay(1);
  }

  void printRecv() {
    Serial.print("Recv Buffer: ");
    Serial.println(iotSerialRecv);
    Serial.print("Serial Buffer: ");
    Serial.println(SerialIoT.available());

    String myString = iotSerialRecv;
    byte buffer[myString.length() + 1];

    myString.getBytes(buffer, sizeof(buffer));

    for (int i = 0; i < sizeof(buffer); i++) {
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }

    Serial.println();
  }
};

extern IoT iot;

#endif

// A  T  +  C  S  Q  +  C  S  Q  :  _  2  4  ,  9  9  O  K
// 41 54 2B 43 53 51 2B 43 53 51 3A 20 32 34 2C 39 39 4F 4B 0
