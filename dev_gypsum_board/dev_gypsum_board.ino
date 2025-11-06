void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  Serial.println(1023 - analogRead(A0));
  delay(1000);
}
