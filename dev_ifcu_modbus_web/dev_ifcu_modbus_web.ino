#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "IFCU.h"
#include "Globals.h"
#include "WebClient.h"
#include "Queue.h"

WebClient webClient;

IFCU ifcu(31);

Queue queue;

void setup() {
  Serial.begin(9600);
  WebClientSerial.begin(115200);

  if (!mbClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1) {};
  }

  delay(1000);
}


void loop() {
  webClient.recvBuffer();
  ifcu.listen();

  delay(10);
}