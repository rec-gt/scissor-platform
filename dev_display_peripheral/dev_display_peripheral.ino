#include "DisplayOLED.h"
#define PERIPHERAL_SERIAL Serial3

DisplayOLED displayOLED;

String buffer = "";

unsigned long prevMillis = 0;

void setup() {
  Serial.begin(9600);
  PERIPHERAL_SERIAL.begin(9600);
  displayOLED.init();
}

void asyncHeartbeat() {
  if (millis() - prevMillis > 1000) {
    Serial.println("send heartbeat");
    Serial.flush();
    PERIPHERAL_SERIAL.println("HB");
    PERIPHERAL_SERIAL.flush();
    prevMillis = millis();
  }
}

void listen() {
  while (PERIPHERAL_SERIAL.available() > 0) {

    char _byte = PERIPHERAL_SERIAL.read();

    if (_byte != '\r' && _byte != '\n') {
      buffer += _byte;
    }

    if (_byte == '\r') {
      const char* charPtr1;
      const char* charPtr2;
      const char* charPtr3;
      Serial.println(buffer);

      int firstDelimiterIndex = buffer.indexOf(';');

      if (firstDelimiterIndex != -1) {
        String firstPart = buffer.substring(0, firstDelimiterIndex);
        charPtr1 = firstPart.c_str();

        int secondDelimiterIndex = buffer.indexOf(';', firstDelimiterIndex + 1);
        if (secondDelimiterIndex != -1) {
          String secondPart = buffer.substring(firstDelimiterIndex + 1, secondDelimiterIndex);
          charPtr2 = secondPart.c_str();

          String thirdPart = buffer.substring(secondDelimiterIndex + 1);
          charPtr3 = thirdPart.c_str();
        }
      }

      displayOLED.print(charPtr1, charPtr2, charPtr3);
      buffer = "";
    }

    delay(1);
  }
  Serial.println();
}

void loop() {
  listen();
  asyncHeartbeat();
  delay(10);
}
