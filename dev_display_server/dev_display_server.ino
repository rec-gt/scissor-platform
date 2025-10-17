#include "DisplayOLED.h"
#include "SerialRecv.h"
#include <avr/wdt.h>

DisplayOLED displayOLED;
SerialRecv serialRecv;

uint16_t hbCnt = 0;

void (*resetFunc)(void) = 0;

void autoReset() {
  if (hbCnt < 1800) {  // 7200 = 1hr, 3600 = 30mins, 1800 = 15mins, 900 = 7.5mins
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
    serialRecv.aos[3],
    serialRecv.imei);

  // serialRecv.imei[0],
  // serialRecv.imei[1],
  // serialRecv.imei[2],
  // serialRecv.imei[3],
  // serialRecv.imei[4],
  // serialRecv.imei[5],
  // serialRecv.imei[6],
  // serialRecv.imei[7],
  // serialRecv.imei[8],
  // serialRecv.imei[9],
  // serialRecv.imei[10],
  // serialRecv.imei[11],
  // serialRecv.imei[12],
  // serialRecv.imei[13],
  // serialRecv.imei[14],
  // serialRecv.imei[15],
  autoReset();
  delay(500);
}
