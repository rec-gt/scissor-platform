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

  void draw() {
    u8g2.firstPage();

    do {
      u8g2.setCursor(0, 10);
      u8g2.print(F("Sensor 1"));
      u8g2.drawLine(0, 16, 128, 16);
      
      u8g2.setCursor(0, 26);
      u8g2.print(F("Sensor 2"));
      u8g2.drawLine(0, 32, 128, 32);
      
      u8g2.setCursor(0, 42);
      u8g2.print(F("Sensor 3"));
      u8g2.drawLine(0, 48, 128, 48);
    } while (u8g2.nextPage());
  }

  ~DisplayOLED(){};
};

extern DisplayOLED displayOLED;

#endif
