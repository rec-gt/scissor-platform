#include <WiFi.h>
#include "Globals.h"
#include "AutoTimer.h"

#ifndef WiFiBroker_H
#define WiFiBroker_H

const char* ssid = "iFCU-16f-test-ckyt";
const char* password = "iFCU-16f-test-ckyt";
const char* masterIP = "192.168.1.100";
const int masterPort = 80;

WiFiClient client;

class WiFiBroker {
private:
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
    this->sendDataToMaster();
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
    String data = "Hello from slave: " + String(millis());
    client.println(data);

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