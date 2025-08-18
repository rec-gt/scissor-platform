#include "Arduino.h"
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"

#ifndef displayOLED_h
#define displayOLED_h

U8G2_SSD1309_128X64_NONAME0_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

class DisplayOLED {
private:

public:
  bool hbToggle = true;

  DisplayOLED() {}

  void init() {
    if (!u8g2.begin()) {
      Serial.println("Display Failed");
      while (1) {}
    }

    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_profont10_tf);
    u8g2.clearDisplay();
  }

  void draw() {
    u8g2.firstPage();
    do {
      int y = 7;
      u8g2.drawStr(0, y, "RGT NBIoT Controller");
      u8g2.drawStr(108, y, "V2.0");
      u8g2.drawLine(0, 9, 128, 9);

      /*=== DI ===*/
      y = 18;
      u8g2.drawStr(0, y, "Di");
      u8g2.drawStr(12, y, "0");
      u8g2.drawStr(17, y, "0");
      u8g2.drawStr(22, y, "0");
      u8g2.drawStr(27, y, "0");
      u8g2.drawStr(32, y, "0");
      u8g2.drawStr(37, y, "0");
      u8g2.drawStr(42, y, "0");
      u8g2.drawStr(47, y, "0");

      /*=== DO ===*/
      u8g2.drawStr(75, y, "Do");
      u8g2.drawStr(87, y, "0");
      u8g2.drawStr(92, y, "0");
      u8g2.drawStr(97, y, "0");
      u8g2.drawStr(102, y, "0");
      u8g2.drawStr(107, y, "0");
      u8g2.drawStr(112, y, "0");
      u8g2.drawStr(117, y, "0");
      u8g2.drawStr(122, y, "0");
      u8g2.drawLine(0, 20, 128, 20);

      /*=== AI ===*/
      y = 29;
      u8g2.drawStr(0, y, "Ai");
      u8g2.drawStr(12, y, "99999");
      u8g2.drawStr(42, y, "99999");
      u8g2.drawStr(72, y, "99999");
      u8g2.drawStr(102, y, "99999");
      y = 37;
      u8g2.drawStr(12, y, "99999");
      u8g2.drawStr(42, y, "99999");
      u8g2.drawStr(72, y, "99999");
      u8g2.drawStr(102, y, "99999");
      y = 45;
      u8g2.drawStr(12, y, "99999");
      u8g2.drawStr(42, y, "99999");
      u8g2.drawStr(72, y, "99999");
      u8g2.drawStr(102, y, "99999");
      u8g2.drawLine(0, 47, 128, 47);

      /*=== AO ===*/
      y = 56;
      u8g2.drawStr(0, y, "Ao");
      u8g2.drawStr(12, y, "99999");
      u8g2.drawStr(42, y, "99999");
      u8g2.drawStr(76, y, "IoT CSQ");
      u8g2.drawStr(116, y, "22");
      y = 64;
      u8g2.drawStr(12, y, "99999");
      u8g2.drawStr(42, y, "99999");
      u8g2.drawStr(76, y, this->hbToggle ? "Hrb" : "   ");
      u8g2.drawStr(96, y, "CONN");
      u8g2.drawStr(121, y, "C");
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
