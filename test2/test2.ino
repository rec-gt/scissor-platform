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
    Serial.print(c);
  }

  if (cmd != "") {
    if (cmd == "RESET") {
      iotConnState = IOT_STATE_WAITING_INIT;
      cmd = "";
    }
    if (cmd == "AT+CSQ") {
      iot.printlnFlush(cmd);
      cmd = "";
    }
  }
}

void loop() {
  serialInput();

  // if (millis() - prevMillis1 > 300) {
  //   prevMillis1 = millis();
  //   Serial.print("iotConnState: ");
  //   Serial.println(iotConnState);
  //   Serial.print("iotResetState: ");
  //   Serial.println(iotResetState);
  // }

  iot.loop();
}
