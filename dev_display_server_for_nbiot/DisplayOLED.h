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
      u8g2.drawStr(0, 7, "RGT NBIoT Controller v2.0");
      u8g2.drawLine(0, 9, 128, 9);

      u8g2.drawStr(0, 18, "Di");
      u8g2.drawStr(12, 18, "00000000");
      u8g2.drawStr(72, 18, "Do");
      u8g2.drawStr(84, 18, "00000000");
      u8g2.drawLine(0, 19, 128, 19);

      u8g2.drawStr(0, 29, "Ai");
      u8g2.drawStr(12, 29, "    0     0     0     0");
      u8g2.drawStr(12, 37, "    0     0     0     0");
      u8g2.drawStr(12, 45, "    0     0     0     0");
      u8g2.drawLine(0, 47, 128, 47);

      u8g2.drawStr(0, 56, "Ao");
      u8g2.drawStr(12, 56, "    0     0");
      u8g2.drawStr(12, 64, "    0     0");
      u8g2.drawStr(76, 56, "IoT CSQ 22");
      u8g2.drawStr(76, 64, "+HB CONN C");
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
