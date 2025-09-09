void setup() {
  Serial.begin(9600);
  for (int i = 1; i <= 53; i++) {
    pinMode(i, INPUT);
  }
}

void loop() {
  for (int i = 1; i <= 53; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(digitalRead(i));
  }
  while (1) {}
}
