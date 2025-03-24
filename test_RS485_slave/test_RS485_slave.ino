#define RS485_Serial Serial3
#define RS485_CTRL_PIN 31


String readString;

void setup() {
  Serial.begin(9600);
  RS485_Serial.begin(9600);
  pinMode(RS485_CTRL_PIN, OUTPUT);
  digitalWrite(RS485_CTRL_PIN, LOW);
}

void loop() {
  listenReceived();
  Serial.println(readString);
  Serial.println("------");
  delay(500);
}

void listenReceived() {
  readString = "";
  while (RS485_Serial.available()) {
    readString += (char)RS485_Serial.read();
  }
}