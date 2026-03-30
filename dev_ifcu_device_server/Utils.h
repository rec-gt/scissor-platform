#ifndef Utils_H
#define Utils_H
#include "esp_mac.h"

class Utils {

public:
  void listenSerial() {
    while (Serial.available()) {
      char c = Serial.read();
      QUEUE += c;
    }
  }

  void printMACAddress() {
    uint8_t mac[6] = { 0 };
    esp_err_t ret = ESP_OK;
    ret = esp_read_mac(mac, ESP_MAC_EFUSE_FACTORY);
    if (ret != ESP_OK) {
      Serial.printf("Failed to get base MAC address from EFUSE BLK0. (%s)", esp_err_to_name(ret));
    } else {
      Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
  }
};

extern Utils utils;

#endif