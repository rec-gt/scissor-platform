
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  // Serial1.print("AT");
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

  if (Serial1.available()) {
    String str = Serial1.readString();
    Serial.println(str);
    if (str == "+QNBIOTEVENT: \"ENTER DEEPSLEEP\"") {
      Serial.println("X");
    }
  }
  delay(1000);
}