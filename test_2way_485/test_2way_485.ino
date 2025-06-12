void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  while (Serial.available()) {
    Serial1.print(255);
  }
  while (Serial1.available()) {
    Serial.println(Serial1.read());
  }
  delay(1000);
}
