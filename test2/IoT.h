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

  void handleReset() {
    // 1. 斷電，重新上電
    if (iotResetState == IOT_STATE_WAITING_RESET_HARDWARE) {
      digitalWrite(24, LOW);
      if (iotTimer.autoExpired(3000)) {
        digitalWrite(24, HIGH);
        iotResetState = IOT_STATE_FINISH_RESET_HARDWARE;
      }
    }

    // 2. software reboot
    if (iotResetState == IOT_STATE_FINISH_RESET_HARDWARE) {
      if (iotTimer.autoExpired(3000)) {
        iotResetState = IOT_STATE_WAITING_RESET_SOFTWARE;
        this->printlnFlush(F("AT+CFUN=1,1"));
      }
    }

    if (iotResetState == IOT_STATE_WAITING_RESET_SOFTWARE) {
      if (iotTimer.autoExpired(3000)) {
        iotResetState = IOT_STATE_FINISH_RESET_SOFTWARE;
        iotConnState = IOT_STATE_FINISH_RESET;
        this->printlnFlush(F("ATI"));
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
    Serial.println(iotResetState);
    if (iotConnState == IOT_STATE_WAITING_INIT) {
      iotConnState = IOT_STATE_WAITING_RESET;
    }

    // =================
    if (iotConnState == IOT_STATE_WAITING_RESET) {
      this->handleReset();
    }
    
    this->listenSerial();
    this->handleResponse();
  }

  void printlnFlush(const String& cmd) {
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
