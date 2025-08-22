#include "U8g2lib.h"

#ifndef displayOLED_h
#define displayOLED_h

U8G2_SSD1309_128X64_NONAME0_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

class DisplayOLED {
private:
  String padString(String str, int length) {
    int originalLength = str.length();

    while (str.length() < length) {
      str += " ";
    }

    return str;
  }

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

  void draw(
    byte iotConn,
    byte iotCsq,
    byte diValue,
    byte doValue,
    uint16_t ai1,
    uint16_t ai2,
    uint16_t ai3,
    uint16_t ai4,
    uint16_t ai5,
    uint16_t ai6,
    uint16_t ai7,
    uint16_t ai8,
    uint16_t ai9,
    uint16_t ai10,
    uint16_t ai11,
    uint16_t ai12,
    uint16_t ao1,
    uint16_t ao2,
    uint16_t ao3,
    uint16_t ao4) {
    u8g2.firstPage();
    do {
      int y = 7;
      u8g2.drawStr(0, y, "RGT NBIoT Controller");
      u8g2.drawStr(108, y, "V2.0");
      u8g2.drawLine(0, 8, 128, 8);

      /*=== DI ===*/
      y = 17;

      u8g2.drawStr(0, y, "Di");
      for (size_t i = 0; i < 8; i++) {
        u8g2.drawStr(12 + 5 * i, y, bitRead(diValue, i) ? "1" : "0");
      }

      /*=== DO ===*/
      u8g2.drawStr(75, y, "Do");
      for (size_t i = 0; i < 8; i++) {
        u8g2.drawStr(87 + 5 * i, y, bitRead(doValue, i) ? "1" : "0");
      }
      u8g2.drawLine(0, 18, 128, 18);

      /*=== AI ===*/
      y = 27;
      u8g2.drawStr(0, y, "Ai");
      u8g2.setCursor(12, y);
      u8g2.print(ai1);
      u8g2.setCursor(42, y);
      u8g2.print(ai2);
      u8g2.setCursor(72, y);
      u8g2.print(ai3);
      u8g2.setCursor(102, y);
      u8g2.print(ai4);
      y = 35;
      u8g2.setCursor(12, y);
      u8g2.print(ai5);
      u8g2.setCursor(42, y);
      u8g2.print(ai6);
      u8g2.setCursor(72, y);
      u8g2.print(ai7);
      u8g2.setCursor(102, y);
      u8g2.print(ai8);
      y = 43;
      u8g2.setCursor(12, y);
      u8g2.print(ai9);
      u8g2.setCursor(42, y);
      u8g2.print(ai10);
      u8g2.setCursor(72, y);
      u8g2.print(ai11);
      u8g2.setCursor(102, y);
      u8g2.print(ai12);
      u8g2.drawLine(0, 44, 128, 44);

      /*=== AO ===*/
      y = 53;
      u8g2.drawStr(0, y, "Ao");
      u8g2.setCursor(12, y);
      u8g2.print(ao1);
      u8g2.setCursor(42, y);
      u8g2.print(ao2);
      u8g2.setCursor(72, y);
      u8g2.print(ao3);
      u8g2.setCursor(102, y);
      u8g2.print(ao4);
      u8g2.drawLine(0, 54, 128, 54);

      y = 63;
      u8g2.drawStr(0, y, "IoT CSQ");
      u8g2.setCursor(30, y);
      u8g2.print(iotCsq);
      u8g2.drawStr(60, y, "CONN");
      u8g2.setCursor(80, y);
      u8g2.print(iotConn);
      u8g2.drawStr(106, y, this->hbToggle ? "Hrb" : "   ");
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
