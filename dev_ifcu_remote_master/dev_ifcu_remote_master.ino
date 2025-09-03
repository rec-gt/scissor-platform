#include <WiFi.h>
#include "WiFiBroker.h"

WiFiBroker wifiBroker;

void setup() {
  Serial.begin(9600);
  wifiBroker.init();
  delay(100);
}

void loop() {
  wifiBroker.loop();
}