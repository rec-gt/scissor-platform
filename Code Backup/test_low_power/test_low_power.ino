#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "PressButton.h"
#include "ContactLine.h"
#include "NBIoT.h"

NBIoT nbiot;

ContactLine line1("line1", 3);
ContactLine line2("line2", 5);
ContactLine line3("line3", 7);
ContactLine line4("line4", 9);
ContactLine line5("line5", 10);

ContactLine lines[] = {
  line1,
  line2,
  line3,
  line4,
  line5,
};

const size_t lines_num = sizeof(lines) / sizeof(lines[0]);

volatile int f_wdt = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Program Start");

  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  delay(100);

  nbiot.init();

  byte interruptPin = 2;
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), wdt_wake, CHANGE);

  digitalWrite(6, HIGH);
  wdt_setup(9);
  lowPower();
}

void loop() {
  if (f_wdt >= 2) {  //唤醒后是会运行loop的，这时候检查唤醒达到多少次了，然后执行自己的代码即可
    digitalWrite(6, LOW);
    checkLines();
    delay(300);
    digitalWrite(6, HIGH);
    f_wdt = 0;
  }
  lowPower();
}

void checkLines(void) {
  delay(100);
  for (size_t i = 0; i < lines_num; i++) {
    lines[i].listen();
    Serial.print(lines[i].getState());
  }
  Serial.println();
  for (size_t i = 0; i < lines_num; i++) {
    if (lines[i].isBreaked()) {
      Serial.print(lines[i].getName());
      Serial.println(" breaked");

      // send MQTT signal to iot platform
      nbiot.publish();
      delay(1000);
      break;
    }
  }
  delay(100);
}

// ==========================

ISR(WDT_vect) {  //看门狗唤醒执行函数
  f_wdt++;       //累加计数
}

void wdt_setup(int ii)  //代替wdt_enable()，并且不要喂狗。
{
  // ii为看门狗超时时间，支持以下数值：0=16毫秒, 1=32毫秒,2=64毫秒,3=128毫秒,4=250毫秒,5=500毫秒,6=1秒 ,7=2秒, 8=4秒, 9=8秒
  byte bb;

  if (ii > 9) { ii = 9; }

  bb = ii & 7;

  if (ii > 7) { bb |= (1 << 5); }

  bb |= (1 << WDCE);

  MCUSR &= ~(1 << WDRF);  //清除复位标志  为了改变WDE或预分频器，我们需要设置WDCE，需要4个时钟周期更新 //int a = 3; a &= 5;//表示 a = a & 5
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = bb;          //设置新的看门狗超时时间
  WDTCSR |= _BV(WDIE);  //设置为定时中断而不是复位
}

void wdt_wake() {
  f_wdt = 2;
}

void lowPower() {
  sei();  // TODO: do not delete
  OFFACDDC();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  MCUCR |= (1 << BODS | 1 << BODSE); /*關閉BODS*/
  MCUCR = MCUCR & (~(1 << BODSE)) | (1 << BODS);
  sleep_cpu(); /*进入睡眠模式*/
}

void OFFACDDC() {
  ADCSRA &= ~(1 << ADEN);
  TWCR &= ~(1 << TWEN);
  delay(1);
}
