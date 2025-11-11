void setup() {
  Serial
  pinMode(30, OUTPUT);
}

void loop() {
  digitalWrite(30, HIGH);
  delay(1000);
  digitalWrite(30, LOW);
  delay(1000);
}