#include "DisplayOLED.h"
#include "SerialRecv.h"

DisplayOLED displayOLED;
SerialRecv serialRecv;

unsigned long prevMillis = 0;

void setup() {
  Serial.begin(115200);
  PeripheralSerial.begin(115200);
  displayOLED.init();
}

void sendHeartbeat() {
  if (millis() - prevMillis > 1000) {
    PeripheralSerial.println("HB");
    PeripheralSerial.flush();
    prevMillis = millis();
  }
}

void loop() {
  serialRecv.listen();
  sendHeartbeat();
  displayOLED.hbToggle = !displayOLED.hbToggle;
  displayOLED.draw(0, 18 + random(5), random(256), random(256), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000), random(20000));
  delay(500);
}
