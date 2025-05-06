#include "Relay.h"
#include "AnalogInput.h"
#include <avr/wdt.h>

Relay relay(10);

AnalogInput ai1(A6);
AnalogInput ai2(A8);
AnalogInput ai3(A10);
AnalogInput ai4(A12);

void setup() {
  analogReference(DEFAULT);

  Serial.begin(9600);

  // === System Init ===
  relay.cut();

  // === watchdog ===
  wdt_enable(WDTO_8S);
}

void loop() {
  delay(1000);

  ai1.listen();
  ai2.listen();
  ai3.listen();
  ai4.listen();

  if ((ai1.getVoltage() < 0.4 && ai3.getVoltage() < 0.4) || (ai2.getVoltage() < 0.4 && ai4.getVoltage() < 0.4)) {
    relay.connect();
  } else {
    relay.cut();
  }

  Serial.print("ai1: ");
  Serial.println(ai1.getVoltage());
  Serial.print("ai2: ");
  Serial.println(ai2.getVoltage());
  Serial.print("ai3: ");
  Serial.println(ai3.getVoltage());
  Serial.print("ai4: ");
  Serial.println(ai4.getVoltage());
  Serial.println();

  wdt_reset();
}
