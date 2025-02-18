#include "displayOLED.h"
DisplayOLED displayOLED;


byte systemStatus = 1;

void setup() {
  Serial.begin(9600);


  if (!displayOLED.init()) {
    // cut relay
  }
}


void loop() {
  char* arr[] = { "中文", "asd", "123" };
  displayOLED.print(arr);
}
