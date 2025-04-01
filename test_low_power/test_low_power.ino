#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "PressButton.h"
#include "ContactLine.h"

ContactLine line1("line1", 8);
ContactLine line2("line2", 9);
ContactLine line3("line3", 10);
ContactLine line4("line4", 11);
ContactLine line5("line5", 12);

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
  Serial.println("Program Start");
  delay(100);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}

void loop() {
  setWatchDog();

  if (wdCntSeconds % 5 == 0) {
    checkLines();
  }
  delay(10);
}

void checkLines(void) {
  for (size_t i = 0; i < lines_num; i++) {
    lines[i].listen();
    Serial.print(lines[i].getState());
  }
  Serial.println();
  for (size_t i = 0; i < lines_num; i++) {
    if (lines[i].isBreaked()) {
      wdt_disable();
      Serial.print(lines[i].getName());
      Serial.println(" breaked");
      // send MQTT signal to iot platform
    }
  }
}
