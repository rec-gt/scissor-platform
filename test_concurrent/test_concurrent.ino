#include "AsyncSerial.h"

AsyncSerial asyncSerial;

String atCommand = "";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void otherJobs() {
  Serial.println("doing other jobs...");
}

void loop() {
  otherJobs();
  asyncSerial.waitMsg();
  delay(200);
}
