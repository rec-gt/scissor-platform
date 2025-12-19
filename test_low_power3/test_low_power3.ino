#include <avr/sleep.h>
#include <avr/wdt.h>

volatile byte wdt_intr_cnt;

void setup() {
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  wdt_setup(9);

  ACSR |= _BV(ACD);  // OFF ACD
  ADCSRA = 0;        // OFF ADC

  sleep_avr();
}

void loop() {
  if (wdt_intr_cnt >= 1) {
    wdt_intr_cnt = 0;
    digitalWrite(6, LOW);
    delay(1000);
    digitalWrite(6, HIGH);
  }

  sleep_avr();
}

ISR(WDT_vect) {  // WDT interrupt, when WDT is interrupted, ++data
  ++wdt_intr_cnt;
}

void wdt_setup(int ii) {  // replace wdt_enable()
  byte bb;

  if (ii > 9) { ii = 9; }
  bb = ii & 7;
  if (ii > 7) { bb |= (1 << 5); }
  bb |= (1 << WDCE);

  MCUSR &= ~(1 << WDRF);
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);  // set to interrupt instead of restart
}

void sleep_avr() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();  // sleep here
}
