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

    this->draw();
    delay(1000);
  }

  void draw() {
    u8g2.firstPage();
    do {
      u8g2.drawStr(2, 6, "RGT NBIoT Controller v2.0");
      u8g2.drawLine(0, 7, 128, 7);

      u8g2.drawStr(0, 6 + 9, "DI:OXOXOXOX");
      u8g2.drawStr(64, 6 + 9, "DO:OXOXOXOX");
      u8g2.drawLine(0, 7 + 9, 128, 7 + 9);

      u8g2.drawStr(0, 6 + 9 * 2, "AO:2000 2000");
      u8g2.drawStr(0, 6 + 9 * 3, "   2000 2000");
      u8g2.drawStr(64, 6 + 9 * 2, "NBIoT: CSQ 22");
      u8g2.drawStr(64, 6 + 9 * 3, "       CONN C");

      u8g2.drawLine(0, 7 + 9 * 3, 128, 7 + 9 * 3);



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
