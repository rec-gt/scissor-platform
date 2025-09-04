#ifndef Debug_H
#define Debug_H

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


{
  unsigned long prevMillis = millis();
  bool toggle = false;

  void debugDO() {
    if (millis() - prevMillis > 5000) {
      if (toggle) {
        for (size_t i = 0; i < DI_NUMS; i++) {
          digitalOutputs[i].connect();
        }
      } else {
        for (size_t i = 0; i < DI_NUMS; i++) {
          digitalOutputs[i].cut();
        }
      }
      prevMillis = millis();
      toggle = !toggle;
    }
  }
}

unsigned long prevMillis = millis();
byte state = 0;

void debugAO() {
  if (millis() - prevMillis > 5000) {
    if (state == 0) {
      for (size_t i = 0; i < AI_NUMS; i++) {
        analogOutputs[i].set(0);
      }
      state = 1;
    } else if (state == 1) {
      for (size_t i = 0; i < AI_NUMS; i++) {
        analogOutputs[i].set(125);
      }
      state = 2;
    } else if (state == 2) {
      for (size_t i = 0; i < AI_NUMS; i++) {
        analogOutputs[i].set(250);
      }
      state = 0;
    }

    prevMillis = millis();
  }
}


#endif