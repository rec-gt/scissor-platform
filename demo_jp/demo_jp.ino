void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  if (Serial1.available()) {
    Serial.print(Serial1.readString());
  }

  // String AT = String(float(random(800, 900) / 10.0));
  // String ST = String(float(random(800, 900) / 10.0));
  // String A = "0";
  // String C = "0";

  String AT = String(float(random(230, 270)/10.0));
  String ST = String(float(random(230, 270)/10.0));
  String A = String(float(random(80, 110)/10.0));
  String C = "1";

  String str = "AT:" + AT + "," + "ST:" + ST + "," + "A:" + A + "," + "C:" + C;
  Serial1.println(str);
  Serial.println(str);
  // Serial1.println("AT:23.5,ST:25.5,S:1,A:13");

  delay(1000);
}
