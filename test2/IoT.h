#include "Globals.h"
#include "AsyncTimer.h"

#ifndef IOT_H
#define IOT_H

AsyncTimer iotTimer(1000);

class IoT {
private:
  uint16_t prevMillis1 = millis();

  void listen() {
    while (SerialIoT.available() > 0) {
      char c = SerialIoT.read();
      iotSerialRecv += c;
    }
  }

  void consume() {
    int delimiterIndex = 0;
    {
      delimiterIndex = iotSerialRecv.indexOf(F("\r\n"));
    }
    if (delimiterIndex != -1) {
      {
        iotExtractedRecv = iotSerialRecv.substring(0, delimiterIndex);
      }
      {
        iotSerialRecv = iotSerialRecv.substring(delimiterIndex + 2);
      }
    } else {
      iotExtractedRecv = F("");
    }
  }

  void stateManagement() {
    if (iotConnState == IOT_STATE_WAITING_INIT) {
      Serial.println(">>> INIT, RESET");
      iotConnState = IOT_STATE_WAITING_RESET;
      iotConnState = IOT_STATE_WAITING_RESET_HARDWARE;
    }

    if (iotConnState == IOT_STATE_WAITING_RESET_HARDWARE) {
      digitalWrite(24, LOW);
      if (iotTimer.asyncDelay(1000)) {
        digitalWrite(24, HIGH);
        iotConnState = IOT_STATE_FINISH_RESET_HARDWARE;
      }
    }

    if (iotConnState == IOT_STATE_FINISH_RESET_HARDWARE) {
      if (iotTimer.asyncDelay(1000)) {
        this->printlnFlush(F("AT+CFUN=1,1"));
        iotConnState = IOT_STATE_WAITING_RESET_SOFTWARE;
      }
    }

    if (iotConnState == IOT_STATE_WAITING_RESET_SOFTWARE) {
      iotCmpStr = F("RDY");
      iotCmpStrIdx = iotExtractedRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        Serial.println(F("\r\n>>> IOT READY"));
        iotConnState = IOT_STATE_FINISH_RESET_SOFTWARE;
        iotConnState = IOT_STATE_FINISH_RESET;
        this->printlnFlush(F("ATI"));
      }
    }

    if (iotConnState == IOT_STATE_FINISH_RESET) {
    }
  }

public:
  void init() {
    SerialIoT.begin(115200);
    pinMode(24, OUTPUT);
    digitalWrite(24, HIGH);
    iotConnState = IOT_STATE_WAITING_INIT;
  }

  void loop() {
    this->listen();
    this->consume();

    this->printExtractedRecv();

    this->stateManagement();
  }

  void printExtractedRecv() {
    if (iotExtractedRecv != F("")) {
      Serial.print("[[");
      Serial.print(iotExtractedRecv);
      Serial.println("]]");
    }
  }

  void printlnFlush(const String& cmd) {
    SerialIoT.println(cmd);
    SerialIoT.flush();
    delay(1);
  }

  void printRecv() {
    Serial.print("Recv Buffer: ");
    Serial.println(iotSerialRecv);
    Serial.print("Conn State: ");
    Serial.println(iotConnState);
    Serial.print("Reset State: ");
    Serial.println(iotConnState);
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
