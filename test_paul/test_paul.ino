#include "Relay.h"
#include "AnalogInput.h"
#include <avr/wdt.h>

Relay relay1(3);
Relay relay2(4);

AnalogInput ai1(A0);
AnalogInput ai2(A1);
AnalogInput ai3(A2);
AnalogInput ai4(A3);

void setup() {
  analogReference(EXTERNAL);

  Serial.begin(9600);

  // === System Starting ===
  // relay1.cut();
  // relay2.cut();

  // === watchdog ===
  // wdt_enable(WDTO_8S);
}

void loop() {
  delay(500);
  ai1.listen();
  ai2.listen();
  ai3.listen();
  ai4.listen();

  Serial.println(ai1.getVoltage());
  Serial.println(ai2.getVoltage());
  Serial.println(ai3.getVoltage());
  Serial.println(ai4.getVoltage());

  // wdt_reset();
}
