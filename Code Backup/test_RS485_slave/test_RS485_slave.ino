void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  while (Serial1.available()) {
    Serial.println((char)Serial1.read());
  }

  delay(1000);
  Serial1.print("p");
}
