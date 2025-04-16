  float overSamplingRead() {
    float avg = 0;
    for (int i = 0; i < 64; i++) {
      avg += analogRead(this->pin);
    };
    avg = (avg + 8) / 16;
    return avg;
  }

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
