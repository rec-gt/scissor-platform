#include "Arduino.h"
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"

#ifndef displayOLED_h
#define displayOLED_h
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

class DisplayOLED {
private:
  int lastState = 0;

public:
  DisplayOLED() {}

  void init() {
    if (!u8g2.begin()) {
      Serial.println("Display Failed");
      while (1) {}  // Block the whole process
    }

    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_unifont_t_chinese1);
    u8g2.setFontDirection(0);
    u8g2.clearDisplay();
    this->print("", "正在加載", "", 1);
    delay(1500);
  }

  char* concatChar(char* a, char* b) {
    char* newChar = new char[strlen(a) + strlen(b) + 1];
    strcpy(newChar, a);
    strcat(newChar, b);
    return newChar;
  }

  char* num2Char(int num) {
    char cstr[16];
    char* newChar = itoa(num, cstr, 10);
    return newChar;
  }

  void print(char* a, char* b, char* c, int currState) {
    if (currState == lastState) {
      return;
    } else {
      lastState = currState;

      u8g2.clearBuffer();
      u8g2.setCursor(0, 18);
      u8g2.print(a);
      u8g2.setCursor(0, 36);
      u8g2.print(b);
      u8g2.setCursor(0, 54);
      u8g2.print(c);
      u8g2.sendBuffer();
    }
  }

  ~DisplayOLED() {}
};

extern DisplayOLED displayOLED;

#endif
