
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  if (Serial1.available()) {
    const int bufferSize = 64;
    byte buffer[bufferSize];
    int bytesRead = Serial1.readBytes(buffer, bufferSize);
    Serial.println(bytesRead);
  }
}