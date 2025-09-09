float overSamplingRead() {
  float avg = 0;
  for (int i = 0; i < 256; i++) {
    avg += analogRead(A11);
  };
  avg = (avg + 8) / 256;
  return avg;
}

void setup() {
  Serial.begin(9600);
  pinMode(A11, INPUT);
}

void loop() {
  float reading = overSamplingRead();
  Serial.println(reading);
  // Serial.println(analogRead(A11));
  delay(1000);
}
