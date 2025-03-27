void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(31, OUTPUT);
  digitalWrite(31, LOW);
}

byte i = 0;
void loop() {
  if (i == 0) {
    Serial1.print("[FPCP6B465800D87F]");
    i++;
  }

  while (Serial1.available()) {
    Serial.print((char)Serial1.read());
  }
  Serial.println();
  delay(1000);
}
