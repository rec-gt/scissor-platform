#include "DisplayOLED.h"
#include "SerialRecv.h"

DisplayOLED displayOLED;
SerialRecv serialRecv;

unsigned long prevMillis = 0;

void setup() {
  Serial.begin(9600);
  PeripheralSerial.begin(9600);
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
  displayOLED.draw(
    serialRecv.nbiotConn,
    serialRecv.nbiotCsq,
    serialRecv.dis,
    serialRecv.dos,
    serialRecv.ais[0],
    serialRecv.ais[1],
    serialRecv.ais[2],
    serialRecv.ais[3],
    serialRecv.ais[4],
    serialRecv.ais[5],
    serialRecv.ais[6],
    serialRecv.ais[7],
    serialRecv.ais[8],
    serialRecv.ais[9],
    serialRecv.ais[10],
    serialRecv.ais[11],
    serialRecv.aos[0],
    serialRecv.aos[1],
    serialRecv.aos[2],
    serialRecv.aos[3]);
  delay(500);
}
