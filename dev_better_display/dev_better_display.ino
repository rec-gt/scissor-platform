#include "Watchdog.h"

String buffer = "";
String recv = "";

Watchdog OLEDWatchdog(10000);

byte OLEDResetPin = 30;

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  pinMode(OLEDResetPin, OUTPUT);
  digitalWrite(OLEDResetPin, HIGH);

  OLEDWatchdog.enable();
  OLEDWatchdog.setCallback([]() {
    digitalWrite(OLEDResetPin, LOW);
    delay(1000);
    digitalWrite(OLEDResetPin, HIGH);
    Serial.println("WATCHDOG RESET OLED");
  });
}

void sendToDisplay() {
  if (buffer.length() > 0) {
    Serial.println(buffer);
    Serial.flush();
    Serial3.println(buffer);
    Serial3.flush();
    buffer = "";
  }
}

void overWhelmingDisplay() {
  String strArr[] = { "STR_LSAKJJHL_1", "STR_SAIOPIDUSS_2;", "STRASODJ_STESS_3;STRASODJ_STESS_3", "STR_STSDAASESS_4;STRASODJ_STESS_4;", "STRASDSD_STESS_5;STRASDSD_STESS_5;STRASDSD_STESS_5", "STR_STASDSADESS_6;STR_STASDSADESS_6;STR_STASDSADESS_6;", "STRDGDFS_STESS_7;STRDGDFS_STESS_7;STRDGDFS_STESS_7;STRDGDFS_STESS_7", "STR_STFGFGESS_8;STR_STFGFGESS_8;STR_STFGFGESS_8", "STRSGDSFG_STESS_9", "STR_STEFGDFGSS_10", "STR_STFDGDFGFDGESS_11", "ASDSFDSSDGDDF", "IOASDOIASJFOIJ", "IUAESFAIUD" };
  buffer = strArr[random() % 13];
  sendToDisplay();
}

void listenHeartbeat() {
  if (Serial3.available() > 0) {
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
}

void loop() {
  OLEDWatchdog.monitor();
  overWhelmingDisplay();
  listenHeartbeat();
  Serial.println("[STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST]");
  // Serial.flush();
  delay(11);
}
