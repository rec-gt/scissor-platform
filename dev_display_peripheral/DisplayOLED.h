#include "Arduino.h"
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"

#ifndef displayOLED_h
#define displayOLED_h

U8G2_SSD1309_128X64_NONAME0_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

class DisplayOLED {
private:
  int lastState = 0;
  bool initSuccess = false;

public:
  DisplayOLED() {}

  void init() {
    if (!u8g2.begin()) {
      Serial.println("Display Failed");
      while (1) {}
    }

    this->initSuccess = true;

    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_unifont_t_chinese1);
    u8g2.setFontDirection(0);
    u8g2.clearDisplay();
  }

  void print(char* line1, char* line2, char* line3, int currState) {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 18);
    u8g2.print(line1);
    u8g2.setCursor(0, 40);
    u8g2.print(line2);
    u8g2.setCursor(0, 62);
    u8g2.print(line3);
    u8g2.sendBuffer();
    // return;
    // if (!this->initSuccess) {
    //   return;
    // }

    // if (currState != this->lastState) {
    //   this->lastState = currState;

    //   u8g2.clearBuffer();
    //   u8g2.setCursor(0, 18);
    //   u8g2.print(line1);
    //   u8g2.setCursor(0, 40);
    //   u8g2.print(line2);
    //   u8g2.setCursor(0, 62);
    //   u8g2.print(line3);
    //   u8g2.sendBuffer();
    // }
  }

  ~DisplayOLED(){};
};

extern DisplayOLED displayOLED;

#endif
