#include "Globals.h"
#include "IoT.h"

IoT iot;

void setup() {
  Serial.begin(9600);
  iot.init();
}

String cmd = "";

uint16_t prevMillis1 = 0;

void serialInput() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c != '\r' && c != '\n') {
      cmd += c;
    }
    if (c == '\r') {
      if (cmd == "RESET") {
        iotConnState = IOT_STATE_WAITING_INIT;
      } else {
        iot.printlnFlush(cmd);
      }
      cmd = "";
    }
  }
}

void loop() {
  serialInput();

  if (millis() - prevMillis1 > 1000) {
    prevMillis1 = millis();
    Serial.print("cmd: ");
    Serial.println(cmd);
  }

  iot.loop();
}
