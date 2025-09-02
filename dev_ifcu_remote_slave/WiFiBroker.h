#include <WiFi.h>
#include "Globals.h"
#include "AutoTimer.h"

#ifndef WiFiBroker_H
#define WiFiBroker_H

const char* ssid = "ifcu-remote-16f-test";
const char* password = "ifcu-remote-16f-test";
const char* masterIP = "192.168.1.100";
const int masterPort = 80;

WiFiClient client;

AutoTimer wifiTimer;

class WiFiBroker {
private:
  String dataToMaster = "";
public:
  WiFiBroker(){};

  void init() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("Slave IP: ");
    Serial.println(WiFi.localIP());
  }

  void loop() {
    this->handleReconnect();
    if (wifiTimer.autoExpire(1000)) {
      this->sendDataToMaster();
      this->recvDataFromMaster();
    }
  }

  void handleReconnect() {
    if (!client.connected()) {
      Serial.println("Connecting to master...");
      if (client.connect(masterIP, masterPort)) {
        Serial.println("Connected to master");
      } else {
        Serial.println("Connection failed. Retrying in 5s...");
        delay(5000);
        return;
      }
    }
  }

  void sendDataToMaster() {
    this->dataToMaster = "";
    this->dataToMaster += String(responseValues[0]);
    this->dataToMaster += ",";
    this->dataToMaster += String(responseValues[1]);
    this->dataToMaster += ",";
    this->dataToMaster += String(responseValues[2]);
    this->dataToMaster += ",";
    this->dataToMaster += String(responseValues[3]);
    this->dataToMaster += ",";
    this->dataToMaster += String(responseValues[4]);
    this->dataToMaster += ",";
    this->dataToMaster += String(responseValues[5]);
    this->dataToMaster += ",";
    this->dataToMaster += String(responseValues[6]);
    client.println(this->dataToMaster);
  }

  void recvDataFromMaster() {
    if (client.available()) {
      String response = client.readStringUntil('\n');
      response.trim();
      Serial.println("Received from master: " + response);
    }
  }

  ~WiFiBroker(){};
};

extern WiFiBroker wifiBroker;

#endif