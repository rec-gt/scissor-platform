void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  pinMode(24, OUTPUT);
  digitalWrite(24, HIGH);
}

void loop() {
  Serial1.println("AT+CSQ");
  Serial.println("Send AT");
  delay(1000);

  if (Serial1.available() > 0) {
    while (Serial1.available() > 0) {
      char c = Serial1.read();
      Serial.print(c);
    }
  }
}
