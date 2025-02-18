#include "Arduino.h"
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"

#define SDA 20
#define SCL 21  // SCL/SCK

#define LH1 18  // Line Height or y-position
#define LH2 36
#define LH3 54

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
// U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

enum StateEnum {
  _NULL,
  INIT,
  PRINT_RUNNING,
  PRINT_ERR,
  PRINT_ALLOW_10S,
  PRINT_MSG,
  PRINT_WARNING,
};

class DisplayOLED {
private:
  StateEnum lastState = _NULL;

  void clear() {
    u8g2.clearBuffer();
  }

  void plot(byte lh, char* msg) {
    u8g2.setCursor(0, lh == 0 ? LH1 : (lh == 1 ? LH2 : LH3));
    u8g2.print(msg);
  }

  void send() {
    u8g2.sendBuffer();
  }

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

    this->print(INIT);
    delay(3000);

    return true;
  }

  void print(StateEnum currState, char* addStr = "") {
    if (this->lastState == currState) {
      return;
    }

    this->lastState = currState;

    // handle all plotting here
    this->clear();

    char* newChar;

    switch (currState) {
      case INIT:
        this->plot(1, "正在加載系統...");
        break;
      case PRINT_RUNNING:
        this->plot(1, "系統運作中");
        break;
      case PRINT_ERR:
        newChar = concatChar("感應器", addStr);

        this->plot(0, newChar);
        this->plot(1, "偵測到障礙物");
        this->plot(2, "系統暫停運作！");
        break;
      case PRINT_ALLOW_10S:
        this->plot(0, "系統允許暫時");
        this->plot(1, "運作十秒！");
        break;
      case PRINT_WARNING:
        this->plot(0, "系統暫停運作！");
        break;
      default:
        break;
    }
    delete[] newChar;
    this->send();
  }
};