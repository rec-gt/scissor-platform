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
      String firstPart = "";
      String secondPart = "";
      String thirdPart = "";

      int firstDelimiterIndex = buffer.indexOf(';');

      if (firstDelimiterIndex != -1) {
        firstPart = buffer.substring(0, firstDelimiterIndex);

        int secondDelimiterIndex = buffer.indexOf(';', firstDelimiterIndex + 1);
        if (secondDelimiterIndex != -1) {
          secondPart = buffer.substring(firstDelimiterIndex + 1, secondDelimiterIndex);
          thirdPart = buffer.substring(secondDelimiterIndex + 1);
        }
      } else {
        firstPart = buffer;
      }

      displayOLED.print(firstPart.c_str(), secondPart.c_str(), thirdPart.c_str());

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
