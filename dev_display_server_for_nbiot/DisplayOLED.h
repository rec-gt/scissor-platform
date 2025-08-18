#include "Arduino.h"
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"

#ifndef displayOLED_h
#define displayOLED_h

U8G2_SSD1309_128X64_NONAME0_2_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

class DisplayOLED {
private:

public:
  DisplayOLED() {}

  void init() {
    if (!u8g2.begin()) {
      Serial.println("Display Failed");
      while (1) {}
    }

    // u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_profont10_tf);
    // u8g2.setFont(u8g2_font_unifont_t_chinese1);
    // u8g2.setFontDirection(0);
    u8g2.clearDisplay();

  }

  void draw() {
    u8g2.firstPage();
    do {
      u8g2.drawLine(0, 8, 128, 8);
      u8g2.drawStr(0, 6, "RGT NBIoT Controller v2.0");
    } while (u8g2.nextPage());
  }

  void print(char* line1, char* line2, char* line3) {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 18);
    u8g2.print(line1);
    u8g2.setCursor(0, 40);
    u8g2.print(line2);
    u8g2.setCursor(0, 62);
    u8g2.print(line3);
    u8g2.sendBuffer();
  }

  ~DisplayOLED(){};
};

extern DisplayOLED displayOLED;

#endif
