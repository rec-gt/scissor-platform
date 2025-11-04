union {
  unsigned long a;
  float b;
} u;


void setup() {
  u.a = 0x3F322E3F;
  Serial.println(u.b);
}
void loop() {
  Serial.println(u.b);
  delay(1000);
}
