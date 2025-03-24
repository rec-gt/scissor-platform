#include "RS485.h"

RS485 rs485(31);

void setup() {
  Serial.begin(9600);
  rs485.mode(1);
}

void loop() {
  rs485.sendMsg(String(random(300)));
  delay(1000);
}
