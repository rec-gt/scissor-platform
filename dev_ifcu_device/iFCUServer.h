#ifndef IFCU_SERVER_H
#define IFCU_SERVER_H

#include "./Timer.h"
#include <WebServer.h>

WebServer server(80);

Timer timer;

class IFCUServer {

private:
  void handleDeviceGet() {
    if (server.hasArg("id")) {
      String id = server.arg("id");
      String onOff = server.arg("onOff");
      String mode = server.arg("mode");
      String speed = server.arg("speed");
      String setTemp = server.arg("setTemp");
      String roomTemp = server.arg("roomTemp");
      String isDeviceSynced = server.arg("isDeviceSynced");

      Record* record = recordDB.findDevice(id);
      if (record != nullptr) {
        record->onOff = onOff;
        record->mode = mode;
        record->speed = speed;
        record->setTemp = setTemp;
        record->roomTemp = roomTemp;
        record->isDeviceSynced = isDeviceSynced;
        record->isConnected = ((millis() - (record->lastCommAt)) <= 10000);
        record->lastCommAt = millis();

        server.send(200, "text/plain", record->cmd);
        record->cmd = "";
      } else {
        Serial.println("ID not found, cannot update!");
      }
    } else {
      server.send(400, "text/plain", "Missing parameters.");
    }
  }

  void handleBrowserSet() {
    if (server.hasArg("id") && server.hasArg("cmd")) {
      String id = server.arg("id");
      String cmd = server.arg("cmd");

      Record* record = recordDB.findDevice(id);
      if (record != nullptr) {
        record->cmd += cmd;
        server.send(200, "text/plain", record->id);
      } else {
        Serial.println("ID not found, cannot update!");
      }
    } else {
      server.send(400, "text/plain", "Missing parameters.");
    }
  }

  void handleBrowserGet() {
    server.send(200, "application/json", recordDB.toJSON());
  }
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
    serverPath.concat(F("&isDeviceSynced="));
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

extern IFCUServer iFCUServer;

#endif
