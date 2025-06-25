#include "SystemEnums.h"
#include "ChargingSystem.h"

ChargingSystem chargingSystem;

void setup() {
  Serial.begin(9600);
  LoRaSerial.begin(9600);
}

void loop() {
  chargingSystem.listenPort();

  // String AT = String(float(random(800, 900) / 10.0));
  // String ST = String(float(random(800, 900) / 10.0));
  // String A = "0";
  // String C = "0";
  // String SPT = "80";
  // String S = String(SYS_RUNNING);

  String AT = String(float(random(230, 270) / 10.0));
  String ST = String(float(random(230, 270) / 10.0));
  String A = String(float(random(80, 110) / 10.0));
  String C = "1";
  String SPT = "80";
  String S = String(SYS_RUNNING);

  String str = "AT:" + AT + "," + "ST:" + ST + "," + "A:" + A + "," + "C:" + C + "," + "SPT:" + SPT + "," + "S:" + S;
  LoRaSerial.println(str);
  Serial.println(str);
  // LoRaSerial.println("AT:23.5,ST:25.5,S:1,A:13,SPT:80");

  delay(1000);
}
