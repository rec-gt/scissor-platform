#define TEST_PIN 13

void setup() {
  Serial.begin(9600);
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
}

void loop() {
  for (int i = 0; i < 4; i++) {
    Serial.print(13 + i);
    Serial.print(", ");
    Serial.println(digitalRead(13 + i));
  }
  delay(1000);
}
