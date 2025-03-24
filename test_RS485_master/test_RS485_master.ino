#define RS485_SERIAL Serial3
#define RS485_CTRL_PIN 31

void setup() {
  Serial.begin(9600);
  RS485_SERIAL.begin(9600);
  pinMode(RS485_CTRL_PIN, OUTPUT);
  digitalWrite(RS485_CTRL_PIN, HIGH);
}

void loop() {
  RS485_SERIAL.print("HI");
  delay(1000);
}
