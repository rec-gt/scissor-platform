#include "SystemEnums.h"
#include "ChargingSystem.h"
#include "Utils.h"

ChargingSystem chargingSystem;
Utils utils;

void setup() {
  Serial.begin(9600);
  LoRaSerial.begin(9600);
}

void loop() {
  chargingSystem.listenPort();
  chargingSystem.sendStatus();

  // String AT = String(float(random(800, 900) / 10.0));
  // String ST = String(float(random(800, 900) / 10.0));
  // String A = "0";
  // String C = "0";
  // String SPT = "80";
  // String S = String(SYS_RUNNING);

  // String AT = String(float(random(230, 270) / 10.0));
  // String ST = String(float(random(230, 270) / 10.0));
  // String A = String(float(random(80, 110) / 10.0));
  // String C = "1";
  // String SPT = "80";
  // String S = String(SYS_RUNNING);

  // String str = "AT:" + AT + "," + "ST:" + ST + "," + "A:" + A + "," + "C:" + C + "," + "SPT:" + SPT + "," + "S:" + S;
  // LoRaSerial.println(str);
  // Serial.println(str);

  delay(1000);
}
