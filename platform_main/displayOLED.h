#include "Arduino.h"
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"

#ifndef display_defined
#define display_defined
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

class DisplayOLED {
private:
  byte lastState = 0;

  char* concatChar(char* a, char* b) {
    char* newChar = new char[strlen(a) + strlen(b) + 1];
    strcpy(newChar, a);
    strcat(newChar, b);
    return newChar;
  }
public:
  bool init() {
    if (!u8g2.begin()) {
      Serial.println("Display Failed");
      return false;
    }

    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_unifont_t_chinese1);
    u8g2.setFontDirection(0);
    u8g2.clearDisplay();
    this->print("", "正在加載", "", 1);
    delay(1500);

    return true;
  }

  void print(char* a, char* b, char* c, byte currState) {
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
};

#endif
