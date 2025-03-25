#define BT_Serial Serial1

String response;
void setup() {
  Serial.begin(9600);
  BT_Serial.begin(9600);

  response = "";
  BT_Serial.print("AT");
  delay(1000);
  while (BT_Serial.available()) {
    response += (char)BT_Serial.read();
  }
  Serial.println(response);

  response = "";
  BT_Serial.print("AT+RESET");
  delay(1000);
  while (BT_Serial.available()) {
    response += (char)BT_Serial.read();
  }
  Serial.println(response);


  response = "";
  BT_Serial.print("AT+NAMEWTF");
  delay(1000);
  while (BT_Serial.available()) {
    response += (char)BT_Serial.read();
  }
  Serial.println(response);

  response = "";
  BT_Serial.print("AT+PIN1111");
  delay(1000);
  while (BT_Serial.available()) {
    response += (char)BT_Serial.read();
  }
  Serial.println(response);

  response = "";
  BT_Serial.print("AT+ROLE=M");
  delay(1000);
  while (BT_Serial.available()) {
    response += (char)BT_Serial.read();
  }
  Serial.println(response);
}

void loop() {
  BT_Serial.print(String(random(300)));
  delay(1000);
}