#include <avr/sleep.h>
#include <avr/power.h>

const int interruptPin = 2;
volatile bool wakeUpFlag = false;

void setup() {
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), wakeUp, CHANGE);

  // Set up a timer interrupt to wake up every 5 seconds
  cli();  // Disable interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 31249;                        // Set the compare register value for 5 seconds at 16 MHz clock
  TCCR1B |= (1 << WGM12);               // Configure timer 1 for CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10);  // Set prescaler to 1024 and start the timer
  TIMSK1 |= (1 << OCIE1A);              // Enable timer compare interrupt
  sei();                                // Enable interrupts
}

void loop() {
  if (wakeUpFlag) {
    // Perform tasks when waking up
    // For example, blink an LED
    Serial.println("===");
    Serial.println(_BV(WDRF));
    Serial.println(MCUSR & _BV(WDRF));

    wakeUpFlag = false;  // Reset the flag
  }

  // Enter deep sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}

void wakeUp() {
  // Set the flag to perform tasks
  wakeUpFlag = true;
}

ISR(TIMER1_COMPA_vect) {
  // Timer interrupt service routine
  // Set the flag to wake up the Arduino
  wakeUpFlag = true;
}