#include "DisplayOLED.h"
#include "Sensor.h"

DisplayOLED displayOLED;
Sensor s1(A0);
Sensor s2(A1);
Sensor s3(A2);

void setup() {
  Serial.begin(9600);
  displayOLED.init();
}

void loop() {
  s1.loop();
  s2.loop();
  s3.loop();

  uint16_t r1 = 1023 - s1.reading;
  uint16_t r2 = 1023 - s2.reading;
  uint16_t r3 = 1023 - s3.reading;
  
  displayOLED.draw(r1, r2, r3);
  delay(1000);
}
