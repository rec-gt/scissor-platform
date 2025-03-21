#include <avr/sleep.h>
#include <avr/power.h>

void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}

void loop() {
  sleep_cpu();
}
