#include <avr/sleep.h>
#define LED_PIN (13)
volatile int f_wdt = 0;  //唤醒累加计数
/*看门狗唤醒设置*/
ISR(WDT_vect) {  //看门狗唤醒执行函数
  f_wdt++;       //累加计数
}
void wdt_setup(int ii)  //代替wdt_enable()，并且不要喂狗。
{
  // ii为看门狗超时时间，支持以下数值：0=16毫秒, 1=32毫秒,2=64毫秒,3=128毫秒,4=250毫秒,5=500毫秒,6=1秒 ,7=2秒, 8=4秒, 9=8秒
  byte bb;
  if (ii > 9) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);
  MCUSR &= ~(1 << WDRF);  //清除复位标志  为了改变WDE或预分频器，我们需要设置WDCE，需要4个时钟周期更新 //int a = 3; a &= 5;//表示 a = a & 5
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = bb;          //设置新的看门狗超时时间
  WDTCSR |= _BV(WDIE);  //设置为定时中断而不是复位
}

/*中断唤醒后执行的代码*/
void wakeup() {
  f_wdt++;  //累加计数
}
/*关闭ADC I2C*/
void OFFACDDC() {
  ADCSRA &= ~(1 << ADEN);
  TWCR &= ~(1 << TWEN);
  delay(1);
}

/*开启ADC I2C*/
void ONACDDC() {
  ACSR &= ~_BV(ACIE);
  ACSR &= ~_BV(ACD);
  ADCSRA |= _BV(ADEN);
  ADCSRA |= _BV(ADIF);
  TWCR |= (1 << TWEN);
}

/*休眠函数,最低功耗模式*/
void lowpower() {
  OFFACDDC();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); /*设置睡眠模式为掉电模式*/
  sleep_enable();                      /*启用睡眠模式*/
  MCUCR |= (1 << BODS | 1 << BODSE);
  MCUCR = MCUCR & (~(1 << BODSE)) | (1 << BODS);
  sleep_cpu(); /*进入睡眠模式*/
}
void setup() {
  wdt_setup(9); /*启动看门狗唤醒，3.3V下，耗电会增加到3.5uA左右*/

  //attachInterrupt(0, wakeup, CHANGE);//启用中断唤醒
  /*单独启用中断唤醒，耗电会小于1uA,D2引脚要接个）0.1uf电容到地，否则会出现大量干扰信号产生中断
 ，设置触发D2口的中断方式为电平变化，回调函数为wakeup（），即执行唤醒328p芯片的功能*/

  pinMode(13, OUTPUT);  //指示灯
  lowpower();           //进入休眠
}

void loop() {
  // if (f_wdt >= 2) { //唤醒后是会运行loop的，这时候检查唤醒达到多少次了，然后执行自己的代码即可
  //   digitalWrite(LED_PIN, HIGH); delay(300); digitalWrite(LED_PIN, LOW); //闪灯
  //   f_wdt = 0;//重置唤醒次数计数
  // }
  lowpower();  //再次进入休眠
}
