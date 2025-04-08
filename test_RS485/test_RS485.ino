void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  digitalWrite(30, HIGH);
  digitalWrite(31, HIGH);

  pinMode(28, OUTPUT);
  pinMode(29, OUTPUT);
  digitalWrite(28, LOW);
  digitalWrite(29, LOW);
}

void loop() {
  byte message[13] = { 0xAF, 0xAF, 0x00, 0x00, 0xAF, 0x80, 0x00, 0x02, 0x00, 0x00, 0x8F, 0x0D, 0x0A };
  Serial1.write(message, 13);
  // Serial1.print(123);
  delay(500);

  while (Serial2.available()) {
    Serial.print(", ");
    Serial.print(String(Serial2.read(), HEX));
  }
  Serial.println();
  delay(500);
}
