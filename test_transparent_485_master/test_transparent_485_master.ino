#include "TP_RS485.h"

TP_RS485 tp_rs485(31);

void setup() {
  Serial.begin(9600);
}

void loop() {
  tp_rs485.sendMsg(String(random(300)));
  delay(1000);
}
