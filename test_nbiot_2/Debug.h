#include "AsyncTimer.h"
#ifndef Debug_H
#define Debug_H

AsyncTimer timer;

class Debug {
private:
  unsigned long prevMillis = millis();

public:
  Debug(){};

  int doCnt = 0;
  bool doToggle = true;

  void DO(DigitalOutput* dos) {
    if (timer.autoExpired(300)) {
      this->doToggle ? dos[this->doCnt].connect() : dos[this->doCnt].cut();
      if (this->doCnt++ == DO_NUMS - 1) {
        this->doCnt = 0;
        this->doToggle = !this->doToggle;
      }
    }
  }

  byte aoStrength = 0;  // max 255
  void AO(AnalogOutput* aos) {
    if (timer.autoExpired(1000)) {
      for (size_t i = 0; i < AO_NUMS; i++) {
        aos[i].set(this->aoStrength);
      }
      this->aoStrength += 85;
    }
  }

  ~Debug(){};
};

// void debugDI() {
//   for (size_t i = 0; i < DI_NUM; i++) {
//     digitalInputs[i].listen();
//   }

//   for (size_t i = 0; i < DI_NUM; i++) {
//     Serial.print(i);
//     Serial.print(": ");
//     Serial.print(digitalInputs[i].getState());
//     Serial.println();
//   }
// }


// {
//   unsigned long prevMillis = millis();
//   bool toggle = false;

//   void debugDO() {
//     if (millis() - prevMillis > 5000) {
//       if (toggle) {
//         for (size_t i = 0; i < DI_NUMS; i++) {
//           digitalOutputs[i].connect();
//         }
//       } else {
//         for (size_t i = 0; i < DI_NUMS; i++) {
//           digitalOutputs[i].cut();
//         }
//       }
//       prevMillis = millis();
//       toggle = !toggle;
//     }
//   }
// }

// {
//   unsigned long prevMillis = millis();
//   byte state = 0;

//   void debugAO() {
//     if (millis() - prevMillis > 5000) {
//       if (state == 0) {
//         for (size_t i = 0; i < AO_NUMS; i++) {
//           analogOutputs[i].set(0);
//         }
//         state = 1;
//       } else if (state == 1) {
//         for (size_t i = 0; i < AO_NUMS; i++) {
//           analogOutputs[i].set(125);
//         }
//         state = 2;
//       } else if (state == 2) {
//         for (size_t i = 0; i < AO_NUMS; i++) {
//           analogOutputs[i].set(250);
//         }
//         state = 0;
//       }

//       prevMillis = millis();
//     }
//   }
// }

// {
//   void debugAI() {
//     for (size_t i = 0; i < AI_NUMS; i++) {
//       Serial.print(this->readings[i]);
//       Serial.print(", ");
//     }
//   }
// }

#endif