#include "Watchdog.h"

String buffer = "";
String recv = "";

Watchdog OLEDWatchdog(10000);

byte OLEDResetPin = 30;

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  pinMode(OLEDResetPin, OUTPUT);

  OLEDWatchdog.enable();
  OLEDWatchdog.setCallback([]() {
    Serial.println("[[[RESET OLED DISPLAY]]]");
    digitalWrite(OLEDResetPin, LOW);
    delay(1000);
    digitalWrite(OLEDResetPin, HIGH);
    delay(1000);
  });
}

void sendToDisplay() {
  if (buffer != "") {
    Serial3.println(buffer);
    Serial3.flush();
    buffer = "";
  }
}

byte cnt = 0;
void overwhelming() {
  String strArr[] = { "STRESS_TEST_1", "STRESS_TEST_2;", "STRESS_TEST_3;STRESS_TEST_3", "STRESS_TEST_4;STRESS_TEST_4;", "STRESS_TEST_5;STRESS_TEST_5;STRESS_TEST_5", "STRESS_TEST_6;STRESS_TEST_6;STRESS_TEST_6;", "STRESS_TEST_7;;", ";STRESS_TEST_8;", ";;STRESS_TEST_9", ";STRESS_TEST_10;STRESS_TEST_10", "STRESS_TEST_11;;STRESS_TEST_11" };
  buffer = strArr[cnt];
  if (cnt < 11) {
    cnt++;
  } else {
    cnt = 0;
  }
  sendToDisplay();
}

void listenHeartbeat() {
  while (Serial3.available() > 0) {

    char _byte = Serial3.read();

    if (_byte != '\r' && _byte != '\n') {
      recv += _byte;
    }

    if (_byte == '\r') {
      if (recv == "HB") {
        OLEDWatchdog.pet();
        Serial.println("received heartbeat");
      }

      recv = "";
    }
    delay(1);
  }
}

void loop() {
  OLEDWatchdog.monitor();
  overwhelming();
  listenHeartbeat();
  // Serial.println("[STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST]");
  // Serial.flush();
  delay(100);
}
