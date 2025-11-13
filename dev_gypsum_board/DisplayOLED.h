#include "U8g2lib.h"

#ifndef displayOLED_h
#define displayOLED_h

U8G2_SSD1309_128X64_NONAME0_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

class DisplayOLED {
public:
  DisplayOLED() {}

  void init() {
    Serial.println(F("Display Init"));
    if (!u8g2.begin()) {
      Serial.println(F("Display Failed"));
      while (1) {}
    }

    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.clearDisplay();
  }

  void draw(uint16_t r1, uint16_t r2, uint16_t r3) {
    u8g2.firstPage();

    do {
      u8g2.setCursor(0, 12);
      u8g2.print(F("S1 (Seepage)"));
      u8g2.setCursor(78, 12);
      u8g2.print(":");
      u8g2.setCursor(94, 12);
      u8g2.print(1024);
      u8g2.drawLine(0, 16, 128, 16);

      u8g2.setCursor(0, 28);
      u8g2.print(F("S2 (Moist)"));
      u8g2.setCursor(78, 28);
      u8g2.print(":");
      u8g2.setCursor(94, 28);
      u8g2.print(r2);
      u8g2.drawLine(0, 32, 128, 32);

      u8g2.setCursor(0, 44);
      u8g2.print(F("S3 (Dry)"));
      u8g2.setCursor(78, 44);
      u8g2.print(":");
      u8g2.setCursor(94, 44);
      u8g2.print(r3);
      u8g2.drawLine(0, 48, 128, 48);
    } while (u8g2.nextPage());
  }

  ~DisplayOLED(){};
};

extern DisplayOLED displayOLED;

#endif
