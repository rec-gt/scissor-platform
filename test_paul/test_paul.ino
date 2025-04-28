#include "Relay.h"
#include "AnalogInput.h"
#include <avr/wdt.h>

Relay relay1(13);

AnalogInput ai1(A0);
AnalogInput ai2(A1);
AnalogInput ai3(A2);
AnalogInput ai4(A3);

void setup() {
  analogReference(EXTERNAL);

  Serial.begin(9600);

  // === System Init ===
  relay1.cut();

  // === watchdog ===
  wdt_enable(WDTO_8S);
}

void loop() {
  delay(500);

  ai1.listen();
  ai2.listen();
  ai3.listen();
  ai4.listen();

  if ((ai1.getVoltage() < 0.7 && ai3.getVoltage() < 0.7) || (ai2.getVoltage() < 0.7 && ai4.getVoltage() < 0.7)) {
    relay1.connect();
  }

  wdt_reset();
}
