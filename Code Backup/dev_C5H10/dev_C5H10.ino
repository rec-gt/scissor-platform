void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  
  while (Serial1.available()) {
    char c = Serial1.read();
    Serial.print(c);
    delay(50);
  }
}
