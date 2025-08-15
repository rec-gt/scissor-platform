#ifndef DEBUG_H
#define DEBUG_H

void debugDI() {
  for (size_t i = 0; i < DI_NUM; i++) {
    digitalInputs[i].listen();
  }

  for (size_t i = 0; i < DI_NUM; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(digitalInputs[i].getState());
    Serial.println();
  }
}

void debugDO() {
  for (size_t i = 0; i < DI_NUMS; i++) {
    digitalOutputs[i].connect();
  }

  delay(2000);

  for (size_t i = 0; i < DI_NUMS; i++) {
    digitalOutputs[i].cut();
  }
}

void debugAO() {
  for (size_t i = 0; i < AI_NUMS; i++) {
    analogOutputs[i].set(0);
  }
  delay(1000);

  for (size_t i = 0; i < AI_NUMS; i++) {
    analogOutputs[i].set(50);
  }
  delay(1000);

  for (size_t i = 0; i < AI_NUMS; i++) {
    analogOutputs[i].set(100);
  }
  delay(1000);

  for (size_t i = 0; i < AI_NUMS; i++) {
    analogOutputs[i].set(150);
  }
  delay(1000);

  for (size_t i = 0; i < AI_NUMS; i++) {
    analogOutputs[i].set(200);
  }
  delay(1000);

  for (size_t i = 0; i < AI_NUMS; i++) {
    analogOutputs[i].set(250);
  }
  delay(1000);
}

#endif