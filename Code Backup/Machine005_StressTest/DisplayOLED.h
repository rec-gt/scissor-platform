#include "Arduino.h"
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Utils.h"
#include "Globals.h"

#ifndef displayOLED_h
#define displayOLED_h

#define RESET_PIN 8

U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/10, /* dc=*/9, /* reset=*/RESET_PIN);

class DisplayOLED {
private:
  int lastState = 0;
  bool initSuccess = false;

  void resetDisplay() {
    digitalWrite(RESET_PIN, LOW);
    delay(10);
    digitalWrite(RESET_PIN, HIGH);
    delay(100);
  }

public:
  DisplayOLED() {}

  void init() {
    this->resetDisplay();

    if (!u8g2.begin()) {
      Serial.println("Display Failed");
      while (1) {}  // Block the whole process
    }

    this->initSuccess = true;

    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_unifont_t_chinese1);
    u8g2.setFontDirection(0);
    u8g2.clearDisplay();

    this->print("", "正在加載系統...", "", DISPLAY_SYS_INIT);
    delay(1000);
  }

  void print(char* line1, char* line2, char* line3, int currState) {
    if (!this->initSuccess) {
      return;
    }

    delay(50);
    
    if (currState != this->lastState) {
      this->lastState = currState;

      u8g2.clearBuffer();
      u8g2.setCursor(0, 18);
      u8g2.print(line1);
      u8g2.setCursor(0, 40);
      u8g2.print(line2);
      u8g2.setCursor(0, 62);
      u8g2.print(line3);
      u8g2.sendBuffer();
    }
  }

  ~DisplayOLED(){};
};

extern DisplayOLED displayOLED;

#endif
