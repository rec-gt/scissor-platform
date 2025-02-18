#include "Arduino.h"
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"

#define LH1 18  // Line Height or y-position
#define LH2 36
#define LH3 54

#ifndef display_defined
#define display_defined
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

class DisplayOLED {
private:
  String lastStr;

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
    this->print("1", "a", "中文");
    delay(1500);

    return true;
  }

  void print(char* a, char* b, char* c) {
    byte size = 3;

    char* tmpChar = concatChar(a, b);
    char* finalChar = concatChar(tmpChar, c);

    String currStr(finalChar);

    free(tmpChar);
    free(finalChar);

    Serial.print(currStr);
    Serial.print(" ");
    Serial.println(lastStr);

    if (currStr == lastStr) {
      Serial.println(0);
      return;
    } else {
      lastStr = currStr;

      u8g2.clearBuffer();
      u8g2.setCursor(0, 18);
      u8g2.print(a);
      u8g2.setCursor(0, 36);
      u8g2.print(b);
      u8g2.setCursor(0, 54);
      u8g2.print(c);
      u8g2.sendBuffer();

      Serial.println(1);
    }
  }
};

#endif
