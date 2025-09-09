#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#ifndef Modbus485_h
#define Modbus485_h

#define RE_DE_PIN 22

class Modbus485 {
private:
  unsigned long prevMillis = millis();

public:
  Modbus485(){};

  void setup() {
    pinMode(RE_DE_PIN, OUTPUT);
    digitalWrite(RE_DE_PIN, LOW);  // HIGH = send, LOW = receive
    Serial3.begin(9600);
  };

  void loop() {
    // if (millis() - prevMillis > 1000) {
    //   Serial3.print("q");
    //   prevMillis = millis();
    // }
    while (Serial3.available()) {
      Serial.println((char)Serial3.read());
    }
  }

  ~Modbus485(){};
};

extern Modbus485 modbus485;

#endif
