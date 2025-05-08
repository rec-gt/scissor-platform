#define NBIOT_Serial Serial1

void setup() {
  Serial.begin(4800);
  NBIOT_Serial.begin(4800);
  Serial.println("=== START ===");
}

// AT+CEREG?
// AT+CGATT?
// AT+CIMI
// AT+CGSN=1
// AT+QMTDISC=1
// AT+QMTOPEN=0,"8.210.84.24",1880
// AT+QMTCONN=0,"dev","tswh","1Wo=[6vA0m"
// AT+QMTPUB=0,0,0,0,"rgt/861096060571706",1,"d"


void loop() {
  if (Serial.available()) {
    String cmd = Serial.readString();
    Serial.print("Console: ");
    Serial.println(cmd);
    NBIOT_Serial.println(cmd);
  }

  if (NBIOT_Serial.available()) {
    String res = NBIOT_Serial.readString();
    Serial.println(res);
  }

  // while (NBIOT_Serial.available()) {
  //   byte r = NBIOT_Serial.read();
  //   Serial.print(char(r));
  // }
}