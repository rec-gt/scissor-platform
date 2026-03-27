#include "./Timer.h"
#include <avr/wdt.h>

#ifndef HardWatchdog_h
#define HardWatchdog_h

Timer oneDayTimer(86400000);

class HardWatchdog {
public:
  HardWatchdog(){};

  void init() {
    wdt_enable(WDTO_8S);  // 8秒内無feed即重啓
  }

  void loop() {
    if (!oneDayTimer.isExpired()) {  // 每24小時重啓一次
      wdt_reset();                   // feed the dog
    }
  }

  ~HardWatchdog(){};
};

#endif
