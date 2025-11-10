#include "DisplayOLED.h"

DisplayOLED displayOLED;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  displayOLED.init();
}

void loop() {
  Serial.println(1023 - analogRead(A0));
  displayOLED.draw();
  delay(1000);
}
