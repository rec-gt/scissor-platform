#include "pressButton.h"
PressButton pressButton(30);


void setup() {
  Serial.begin(9600);
}

void loop() {
  pressButton.debounceListen();
  if (pressButton.isPressed()) {
    Serial.println("is pressed");
  }else{
     Serial.println("not pressed");
  }
}
