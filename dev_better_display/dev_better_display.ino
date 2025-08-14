String buffer = "";

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
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
  String strArr[] = { "STR_STESS_1", "STR_STESS_2", "STR_STESS_3", "STR_STESS_4", "STR_STESS_5", "STR_STESS_6", "STR_STESS_7", "STR_STESS_8", "STR_STESS_9", "STR_STESS_10", "STR_STESS_11" };
  buffer = strArr[random() % 11];
  sendToDisplay();
}

void loop() {
  overWhelmingDisplay();
  listenHeartbeat();
  Serial.println("[STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST] [STRESS_TEST]");
  Serial.flush();
  delay(10);
}
