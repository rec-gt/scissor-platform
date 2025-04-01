#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>




byte cnt = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Reseting...");
}

void loop() {

  // Serial.println(_BV(WDCE));
  // Serial.println(1 << WDCE);
  // Serial.println((1 << WDCE) | (1 << WDE));
  // Serial.println(bit(WDIE));
  // Serial.println(_BV(WDIE));


  delay(1000);

  // while (1) {
  //   Serial.println(cnt++);
  //   delay(1000);
  // }
}
