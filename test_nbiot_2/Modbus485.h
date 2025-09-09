#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#ifndef Modbus485_h
#define Modbus485_h

#define RE_DE_PIN 22

class Modbus485 {
private:
  String serialRecv = "";

public:
  Modbus485(){};

  void setup() {
    pinMode(RE_DE_PIN, OUTPUT);
    digitalWrite(RE_DE_PIN, HIGH);  // HIGH = send, LOW = receive
    Serial3.begin(9600, SERIAL_8N1);
  };

  void loop() {
    this->listen();
  }

  void listen() {
    digitalWrite(RE_DE_PIN, LOW);
    delay(1);
    while (Serial3.available()) {
      char c = Serial3.read();
      if (c != '\r' && c != '\n') {
        this->serialRecv += c;
      }
      if (c == '\r') {
        this->answer();
      }
    }
  }

  void answer() {
    int idx = this->serialRecv.indexOf("AT");
    if (idx > -1) {
      digitalWrite(RE_DE_PIN, HIGH);
      delay(1);
      Serial3.println("[Hello from RGT Hello from RGT Hello from RGT]");
      Serial3.flush();
    }
  }

  ~Modbus485(){};
};

extern Modbus485 modbus485;

#endif
