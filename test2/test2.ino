#include "Globals.h"
#include "IoT.h"

IoT iot;

void setup() {
  Serial.begin(9600);
  iot.init();
}

String cmd = "";

uint16_t prevMillis1 = 0;
uint16_t prevMillis2 = 0;

void serialInput() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    cmd += c;
  }

  if (cmd != "") {
    iot.printlnFlush(cmd);
    cmd = "";
  }
}

void loop() {
  serialInput();

  // if (millis() - prevMillis1 > 1000) {
  //   prevMillis1 = millis();
  //   iot.printlnFlush("AT+CSQ");
  // }

  iot.loop();
}
