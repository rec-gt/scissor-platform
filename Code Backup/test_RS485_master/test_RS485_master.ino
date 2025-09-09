#include "RS485.h"

RS485 rs485(12);

void setup() {
  Serial.begin(9600);
  rs485.mode(0);
}

void loop() {
  rs485.waitForMsg();
  Serial.print(rs485.getMsg());
  delay(500);
}
