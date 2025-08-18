#include "DisplayOLED.h"

#define PERIPHERAL_SERIAL Serial

DisplayOLED displayOLED;

unsigned long prevMillis = 0;

String buffer = "";
String line1 = "";
String line2 = "";
String line3 = "";

void setup() {
  Serial.begin(9600);
  PERIPHERAL_SERIAL.begin(9600);
  displayOLED.init();
}

void sendHeartbeat() {
  if (millis() - prevMillis > 1000) {
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
      int delimiterIdx1 = buffer.indexOf(";");

      if (delimiterIdx1 != -1) {
        line1 = buffer.substring(0, delimiterIdx1);

        int delimiterIdx2 = buffer.indexOf(";", delimiterIdx1 + 1);

        if (delimiterIdx2 != -1) {
          line2 = buffer.substring(delimiterIdx1 + 1, delimiterIdx2);
          line3 = buffer.substring(delimiterIdx2 + 1);

          int delimiterIdx3 = buffer.indexOf(";", delimiterIdx2 + 1);
          if (delimiterIdx3 != -1) {
            line3 = buffer.substring(delimiterIdx2 + 1, delimiterIdx3);
          }

        } else {
          line2 = buffer.substring(delimiterIdx1 + 1, delimiterIdx2);
        }

      } else {
        line1 = buffer;
      }

      displayOLED.print(line1.c_str(), line2.c_str(), line3.c_str());
      buffer = "";
      line1 = "";
      line2 = "";
      line3 = "";
    }

    delay(1);
  }
}

void loop() {
  sendHeartbeat();
  displayOLED.hbToggle = !displayOLED.hbToggle;
  displayOLED.draw(random(256), random(256), 20000, 20000, 20000, 20000, 20000, 20000, 20000, 20000, 20000, 20000, 20000, 20000);
  delay(1000);
}
