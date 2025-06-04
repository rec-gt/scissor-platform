#include "AsyncSerial.h"

String atCommand = "";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void otherJobs() {
  Serial.println("doing other jobs...");
}

void asyncReceive() {
  if (Serial1.available() > 0) {
    char incomingByte = Serial1.read();
    if (incomingByte != '\r' && incomingByte != '\n') {
      atCommand += incomingByte;
    }

    if (incomingByte == '\r') {
      handleATCommand(atCommand);
      atCommand = "";
    }
  }
}

void handleATCommand(String command) {
  Serial.print("Received AT command: ");
  Serial.println(command);
}

void loop() {
  otherJobs();
  asyncReceive();
  delay(500);
}
