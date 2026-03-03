void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  Serial2.println("HIHI");
  delay(1000);
}
