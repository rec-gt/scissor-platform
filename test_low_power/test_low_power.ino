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

int wdt = 1;

void setup() {
  Serial.begin(9600);
  Serial.println("Reseting...");

  wdt_enable(WDTO_4S);
}

void loop() {
  for (size_t i; i < 5; i++) {
    lines[i].listen();
  }

  for (size_t i; i < 5; i++) {
    if (lines[i].isBreak()) {
      Serial.print(lines[i].getName());
      Serial.println(" breaked")
      // send MQTT signal to iot platform
    }
  }

  delay(1000);

  wdt_reset();
}
