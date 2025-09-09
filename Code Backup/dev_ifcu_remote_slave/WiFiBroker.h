#include <WiFi.h>
#include "Globals.h"
#include "AutoTimer.h"

#ifndef WiFiBroker_H
#define WiFiBroker_H

const char* ssid = "ifcu-remote-16f-test";
const char* password = "ifcu-remote-16f-test";
const char* masterIP = "192.168.1.2";
const int masterPort = 80;

IPAddress staticIP(192, 168, 1, 101);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiClient client;

AutoTimer wifiTimer;

class WiFiBroker {
private:
  String dataToMaster = "";
public:
  WiFiBroker(){};

  void init() {
    WiFi.mode(WIFI_STA);

    if (!WiFi.config(staticIP, gateway, subnet)) {
      delay(1000);
      Serial.println("Fail to config...");
    }

    WiFi.hostname("IFCU");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("Slave IP: ");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());
  }

  void loop() {
    // if (wifiTimer.autoExpire(1000)) {
    //   this->handleReconnect();
    //   this->sendDataToMaster();
    // }
  }

  void handleReconnect() {
    if (!client.connected()) {
      Serial.println("Connecting to master...");
      if (client.connect(masterIP, masterPort)) {
        Serial.println("Connected to master");
      } else {
        Serial.println("Connection failed. Retrying in 5s...");
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