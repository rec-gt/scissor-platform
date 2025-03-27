#include "RS485.h"

RS485 rs485(31);

void setup() {
  Serial.begin(9600);
  rs485.mode(1);
}

void loop() {
  rs485.mode(1);
  rs485.sendMsg("[FPCP69445800CD39]");
  delay(500);

  rs485.mode(0);
  rs485.waitForMsg();
  Serial.print(rs485.getMsg());
  delay(500);
}
