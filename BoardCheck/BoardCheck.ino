#include "Relay.h"
#include "EncoderSwitch.h"

Relay relay24(24);
Relay relay25(25);
Relay relay26(26);
Relay relay27(27);
Relay relay28(28);
Relay relay29(29);
Relay relay30(30);
Relay relay31(31);

EncoderSwitch sw1(6);
EncoderSwitch sw2(7);
EncoderSwitch sw3(8);
EncoderSwitch sw4(9);

void setup() {
  Serial.begin(9600);

  relay24.connect();
  delay(100);
  relay24.cut();
  delay(100);

  relay25.connect();
  delay(100);
  relay25.cut();
  delay(100);

  relay26.connect();
  delay(100);
  relay26.cut();
  delay(100);

  relay27.connect();
  delay(100);
  relay27.cut();
  delay(100);

  relay28.connect();
  delay(100);
  relay28.cut();
  delay(100);

  relay29.connect();
  delay(100);
  relay29.cut();
  delay(100);

  relay30.connect();
  delay(100);
  relay30.cut();
  delay(100);

  relay31.connect();
  delay(100);
  relay31.cut();
  delay(100);
}

void loop() {
  sw1.listen();
  sw2.listen();
  sw3.listen();
  sw4.listen();

  sw1.print();
  sw2.print();
  sw3.print();
  sw4.print();

  delay(500);
}
