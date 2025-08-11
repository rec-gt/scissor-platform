#include <SPI.h>
#include "DisplayOLED.h"

bool toggle = false;

void setup(void) {
  Serial.begin(9600);
  displayOLED.init();
}

void stressTest() {
  if (toggle) {
    displayOLED.print("中文ASD456英文系統", "英文ASD456中文系統", "中文ASD456英文系統", "英文ASD456中文系統", 2);
  } else {
    displayOLED.print("英文ASD456中文系統", "中文ASD456英文系統", "英文ASD456中文系統", "中文ASD456英文系統", 4);
  }
}

void loop() {
  stressTest();
  Serial.print("");
  delay(10);
}