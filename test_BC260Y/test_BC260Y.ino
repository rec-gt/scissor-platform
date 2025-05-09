#define NBIOT_Serial Serial1

void setup() {
  Serial.begin(4800);
  NBIOT_Serial.begin(4800);
  Serial.println("=== START ===");
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
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
    if (cmd == "s") {
      sendMsg();
    }
  }

  // if (NBIOT_Serial.available()) {
  //   String res = NBIOT_Serial.readString();
  //   Serial.println(res);
  // }
}


void sendMsg() {
  // wakeup();
  NBIOT_Serial.println("AT");
  delay(3000);

  if (NBIOT_Serial.available()) {
    String res = NBIOT_Serial.readString();
    Serial.println(res);
  }
  NBIOT_Serial.println("AT+QMTOPEN=0,\"8.210.84.24\",1880");
  delay(3000);

  if (NBIOT_Serial.available()) {
    String res = NBIOT_Serial.readString();
    Serial.println(res);
  }
  NBIOT_Serial.println("AT+QMTCONN=0,\"dev\",\"tswh\",\"1Wo=[6vA0m\"");
  delay(3000);

  if (NBIOT_Serial.available()) {
    String res = NBIOT_Serial.readString();
    Serial.println(res);
  }
  NBIOT_Serial.println("AT+QMTPUB=0,0,0,0,\"rgt/861096060571706\",1,\"i\"");
  delay(3000);

  if (NBIOT_Serial.available()) {
    String res = NBIOT_Serial.readString();
    Serial.println(res);
  }
}