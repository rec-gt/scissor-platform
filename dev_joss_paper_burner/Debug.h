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


#endif