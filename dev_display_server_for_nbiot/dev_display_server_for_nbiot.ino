#include "DisplayOLED.h"
#include "SerialRecv.h"
#include <avr/wdt.h>

DisplayOLED displayOLED;
SerialRecv serialRecv;

uint16_t hbCnt = 0;

void (*resetFunc)(void) = 0;

void autoReset() {
  if (hbCnt < 10) {
    hbCnt++;
    return;
  }

  resetFunc();
}

void setup() {
  Serial.begin(9600);
  PeripheralSerial.begin(9600);
  displayOLED.init();
}

void loop() {
  serialRecv.listen();
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

  autoReset();

  delay(500);
}
