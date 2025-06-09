#include "NBIoT.h"

NBIoT nbiot;

String atCommand = "";

void setup() {
  Serial.begin(9600);
  NBIoT_Serial.begin(9600);
  delay(300);
}

void otherJobs() {
  Serial.println("doing other jobs...");
}

void loop() {
  nbiot.listen();
  nbiot.waitMsg();
}
