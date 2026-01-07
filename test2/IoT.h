#include "Globals.h"
#include "AsyncTimer.h"

#ifndef IOT_H
#define IOT_H

AsyncTimer iotTimer(1000);

class IoT {
private:
  uint16_t prevMillis1 = millis();

  void clearSerialBuffer() {
    while (SerialIoT.read() > 0) { delay(1); };
  }

  void clearRecvBuffer() {
    iotSerialRecv = F("");
  }

  void listen() {
    while (SerialIoT.available() > 0) {
      char c = SerialIoT.read();
      iotSerialRecv += c;
      // Serial.print(c);

      // if (c != '\r' && c != '\n' && c != ' ') {
      //   iotSerialRecv += c;
      // }

      // if (c == '\r') {
      //   this->handleResponse();
      //   this->clearRecvBuffer();
      // }
    }
  }

  void consume() {
    int delimiterIndex = 0;
    {
      delimiterIndex = iotSerialRecv.indexOf("\r\n");
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

    Serial.print(iotExtractedRecv);
    Serial.println(iotSerialRecv);
  }

  void handleResponse() {
    if (iotResetState == IOT_STATE_WAITING_RESET_SOFTWARE) {
      iotCmpStr = F("RDY");
      iotCmpStrIdx = iotSerialRecv.indexOf(iotCmpStr);
      if (iotCmpStrIdx > -1) {
        Serial.println(F("\r\n>>> IOT READY"));
        iotResetState = IOT_STATE_FINISH_RESET_SOFTWARE;
        iotConnState = IOT_STATE_FINISH_RESET;
        Serial.println("Send ATI");
        this->printlnFlush(F("ATI"));
      }
    }
  }

  void handleReset() {
    // 1. 斷電，重新上電
    if (iotResetState == IOT_STATE_WAITING_RESET_HARDWARE) {
      digitalWrite(24, LOW);
      if (iotTimer.autoExpired(1000)) {
        digitalWrite(24, HIGH);
        iotResetState = IOT_STATE_FINISH_RESET_HARDWARE;
      }
    }

    // 2. software reboot
    if (iotResetState == IOT_STATE_FINISH_RESET_HARDWARE) {
      if (iotTimer.autoExpired(1000)) {
        iotResetState = IOT_STATE_WAITING_RESET_SOFTWARE;
        this->printlnFlush(F("AT+CFUN=1,1"));
      }
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
    // state management
    if (iotConnState == IOT_STATE_WAITING_INIT) {
      Serial.println(">>> RESET");
      iotConnState = IOT_STATE_WAITING_RESET;
      iotResetState = IOT_STATE_WAITING_RESET_HARDWARE;
    }

    // =================
    if (iotConnState == IOT_STATE_WAITING_RESET) {
      this->handleReset();
    }

    this->listen();
    this->consume();
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
    Serial.println(iotResetState);
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
