#include "AsyncSerial.h"

AsyncSerial asyncSerial;

String atCommand = "";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  asyncSerial.init();
  delay(1000);
}

void otherJobs() {
  Serial.println("doing other jobs...");
}

void loop() {
  // otherJobs();
  asyncSerial.listen();
  asyncSerial.waitMsg();
}
