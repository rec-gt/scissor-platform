#include "./Globals.h"
#include <avr/wdt.h>

#ifndef HardWatchdog_h
#define HardWatchdog_h

class HardWatchdog {
public:
  HardWatchdog(){};

  void init() {
    wdt_enable(WDTO_8S);  // 若8秒内無feed(死機)，即重啓
  }

  void loop() {
    if (oneDayTimer.isExpired()) {  // 超過24小時，自殺
      return;
    } else {
      wdt_reset();  // 未過24小時，feed the dog
    }
  }

  ~HardWatchdog(){};
};

#endif
