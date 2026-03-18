#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include "./Timer.h"

Timer timer;

class HttpService {
public:
  void init() {
    http.begin(serverPath.c_str());
  }

  void buildPath() {
    serverPath = serverName;
    serverName.concat(F("?id="));
    serverName.concat(DEVICE_NAME);
    serverName.concat(F("&onOff="));
    serverName.concat(TMP_DATA[0]);
    serverName.concat(F("&mode="));
    serverName.concat(TMP_DATA[1]);
    serverName.concat(F("&speed="));
    serverName.concat(TMP_DATA[2]);
    serverName.concat(F("&setTemp="));
    serverName.concat(TMP_DATA[3]);
  }

  void loop() {
    if (timer.autoTimeout(5000)) {
      if (WiFi.status() == WL_CONNECTED) {
        if (http.connected()) {
          int httpResponseCode = http.GET();
          if (httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println(payload);
          }
          http.end();
        }
      }
    }
  }
};

extern HttpService httpService;

#endif
