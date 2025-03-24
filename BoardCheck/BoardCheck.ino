#include "Relay.h"

Relay relay24(24);
Relay relay25(25);
Relay relay26(26);
Relay relay27(27);
Relay relay28(28);
Relay relay29(29);
Relay relay30(30);
Relay relay31(31);

void setup() {
  Serial.begin(9600);

  relay24.connect();
  delay(1000);
  relay24.cut();
  delay(1000);


  relay25.connect();
  delay(1000);
  relay25.cut();
  delay(1000);


  relay26.connect();
  delay(1000);
  relay26.cut();
  delay(1000);


  relay27.connect();
  delay(1000);
  relay27.cut();
  delay(1000);


  relay28.connect();
  delay(1000);
  relay28.cut();
  delay(1000);


  relay29.connect();
  delay(1000);
  relay29.cut();
  delay(1000);


  relay30.connect();
  delay(1000);
  relay30.cut();
  delay(1000);


  relay31.connect();
  delay(1000);
  relay31.cut();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
}
