#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include "./Timer.h"
#include "./iFCUModbus.h"

Timer timer;

class HttpService {
public:
  void init() {
  }

  void buildPath() {
    serverPath = serverName;
    serverPath.concat(F("?id="));
    serverPath.concat(DEVICE_NAME);
    serverPath.concat(F("&onOff="));
    serverPath.concat((READ_DATA[1] & (1 << 6)) ? 1 : 0);
    serverPath.concat(F("&mode="));
    serverPath.concat(READ_DATA[3]);
    serverPath.concat(F("&speed="));
    serverPath.concat(READ_DATA[4]);
    serverPath.concat(F("&setTemp="));
    serverPath.concat(READ_DATA[6]);
    serverPath.concat(F("&roomTemp="));
    serverPath.concat(READ_DATA[5]);
    serverPath.concat(F("&isSynced="));
    serverPath.concat(isSynced);
    Serial.println(serverPath);
  }

  void loop() {
    /*=== Escape when iFCU Failure ===*/
    if (!ifcuModbus.mbSuccess()) {
      return;
    }

    /*=== 5s update ===*/
    if (timer.autoTimeout(5000)) {
      this->buildPath();
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverPath.c_str());
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
          String payload = http.getString();
          Serial.println("HTTP Response Code: " + String(httpResponseCode));
          Serial.println("Payload: ");
          Serial.println(payload);
          QUEUE = payload;
        } else {
          Serial.println("Error in HTTP request. Response code: " + String(httpResponseCode));
        }

        http.end();
      } else {
        Serial.println("wifi not connected");
      }
    }
  }
};

extern HttpService httpService;

#endif
