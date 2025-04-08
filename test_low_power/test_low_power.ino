#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "PressButton.h"
#include "ContactLine.h"
#include "NBIoT.h"

NBIoT nbiot;

ContactLine line1("line1", 2);
ContactLine line2("line2", 3);
ContactLine line3("line3", 4);
ContactLine line4("line4", 5);
ContactLine line5("line5", 6);

ContactLine lines[] = {
  line1,
  line2,
  line3,
  line4,
  line5
};

const size_t lines_num = sizeof(lines) / sizeof(lines[0]);

volatile byte wdCntSeconds = 0;

ISR(WDT_vect) {
  wdCntSeconds++;
}

void setWatchDog() {
  cli();
  // pat dog
  wdt_reset();
  // reset watchdog reset flag only
  MCUSR &= ~(1 << WDRF);
  // enable watchdog and enable change watchdog
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // set time 8s
  WDTCSR = (1 << WDP2) | (1 << WDP1);
  // enable watchdog interupt
  WDTCSR |= (1 << WDIE);
  sei();
}


void setup() {
  Serial.begin(9600);
  // nbiot.init();…
  Serial.println("Program Start");
  checkLines();
  delay(100);
  wdt_enable(WDTO_8S);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}

void loop() {
}

void checkLines(void) {
  for (size_t i = 0; i < lines_num; i++) {
    lines[i].listen();
    Serial.print(lines[i].getState());
  }
  Serial.println();
  for (size_t i = 0; i < lines_num; i++) {
    if (lines[i].isBreaked()) {
      Serial.print(lines[i].getName());
      Serial.println(" breaked");
      // send MQTT signal to iot platform
    }
  }
}
