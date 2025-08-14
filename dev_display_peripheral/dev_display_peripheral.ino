#include "DisplayOLED.h"

DisplayOLED displayOLED;

String buffer = "";

unsigned long prevMillis = 0;

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  displayOLED.init();
}

void printMsg() {
  Serial.println(buffer);
  const char* charPtr = buffer.c_str();
  displayOLED.print(charPtr, charPtr, charPtr, random() * 10);
}

void clearResBuffer() {
  buffer = "";
}

void asyncHeartbeat() {
  if (millis() - prevMillis > 1000) {
    Serial3.println("HB");
    Serial3.flush();
    prevMillis = millis();
  }
}

void listen() {
  if (Serial3.available() > 0) {
    while (Serial3.available() > 0) {

      char _byte = Serial3.read();

      if (_byte != '\r' && _byte != '\n') {
        buffer += _byte;
      }

      if (_byte == '\r') {
        printMsg();
        clearResBuffer();
      }

      delay(1);
    }
    Serial.println();
  }
}

void loop() {
  listen();
  asyncHeartbeat();
  delay(10);
}
