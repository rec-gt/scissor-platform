void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  Serial1.print(255);
  delay(1000);
}
