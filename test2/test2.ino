byte toggle = LOW;
byte buffer[8] = {};

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  pinMode(30, OUTPUT);
  pinMode(28, OUTPUT);
  digitalWrite(30, toggle);
  delay(300);
}

void loop() {
  if (Serial1.available()) {
    size_t i = 0;

    while (Serial1.available()) {
      Serial.print(Serial1.read());
      Serial.print(", ");
    }

    // while (Serial1.available()) {
    //   byte recv = Serial1.read();
    //   buffer[i++] = recv;
    // }

    // if (buffer[0] == 132) {
    //   //switch to send mode
    //   digitalWrite(28, HIGH);
    //   for (size_t i = 0; i < 8; i++) {
    //     Serial2.print(buffer[i]);
    //   }


    //   for (size_t i = 0; i < 8; i++) {
    //     Serial.print(buffer[i]);
    //     Serial.print(", ");
    //   }


    //   //switch to recv mode
    //   digitalWrite(28, LOW);
    //   if (Serial2.available()) {
    //     for (size_t i = 0; i < 8; i++) {
    //       Serial.print(Serial2.read());
    //     }
    //   }
    // }
  }


  Serial.println();
  delay(1000);
}
