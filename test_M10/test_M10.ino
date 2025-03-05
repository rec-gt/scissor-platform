#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);

void setup() {
  Serial.begin(9600);
  mySerial.begin(460800);
}

void loop() {
  if (mySerial.available() > 0) {
    byte data = mySerial.read();
    if (data == 165 || data == 90 || data == 250 || data == 251) {
      Serial.println(data, HEX);
    }
  }

  delay(2);
}