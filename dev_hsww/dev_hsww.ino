#include "NBIoT.h"
#include "Speaker.h"
#include "System.h"
#include "Enums.h"

NBIoT nbiot;
Speaker speaker;
System system;

void setup() {
  Serial.begin(9600);
  NBIoT_Serial.begin(9600);
  delay(300);
}

void loop() {
  nbiot.listen();
  nbiot.waitMsg();
}
