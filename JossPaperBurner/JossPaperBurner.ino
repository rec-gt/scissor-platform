#include "Relay.h"
#include "AnalogInput.h"
#include <avr/wdt.h>

Relay relay(10);

AnalogInput ai1(A0);
AnalogInput ai2(A2);
AnalogInput ai3(A4);
AnalogInput ai4(A6);

void setup() {
  // analogReference(DEFAULT);
  analogReference(EXTERNAL);

  Serial.begin(9600);

  // === System Init ===
  relay.cut();

  // === watchdog ===
  wdt_enable(WDTO_8S);
}

void loop() {
  delay(500);

  ai1.listen();
  ai2.listen();
  ai3.listen();
  ai4.listen();

  if ((ai1.getVoltage() < 0.4 && ai3.getVoltage() < 0.4) || (ai2.getVoltage() < 0.4 && ai4.getVoltage() < 0.4)) {
    relay.connect();
  } else {
    relay.cut();
  }

  ai1.print();
  ai2.print();
  ai3.print();
  ai4.print();
  Serial.println();

  wdt_reset();
}
