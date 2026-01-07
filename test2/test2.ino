#include "Globals.h"
#include "IoT.h"

IoT iot;

void setup() {
  Serial.begin(9600);
  iot.init();
}

void loop() {

  iot.printlnFlush("AT+CSQ");
  iot.loop();
}
