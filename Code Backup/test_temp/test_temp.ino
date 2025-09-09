void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  int reading = analogRead(A0);
  Serial.println(reading);
  delay(1000);
}
