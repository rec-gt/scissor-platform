#include <WiFi.h>
#include "AutoTimer.h"
#include "SlaveData.h"
#include "DatabaseBroker.h"

#ifndef WiFiBroker_H
#define WiFiBroker_H

#define MAX_SLAVES 5
#define SERVER_PORT 80

AutoTimer wifiTimer;

const char* apSSID = "ifcu-remote-16f-test";
const char* apPassword = "ifcu-remote-16f-test";

IPAddress staticIP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(SERVER_PORT);
WiFiClient slaveClients[MAX_SLAVES];

SlaveData slaveDatabase[MAX_SLAVES] = {
  SlaveData(),
  SlaveData(),
  SlaveData(),
  SlaveData(),
  SlaveData(),
};

DatabaseBroker databaseBroker(slaveDatabase);

class WiFiBroker {
private:
public:
  WiFiBroker(){};

  void init() {
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAPConfig(staticIP, gateway, subnet)) {
      Serial.println("Failed to configure AP static IP");
    }

    WiFi.softAP(apSSID, apPassword);
    Serial.println("Access Point started");
    Serial.print("Master AP IP: ");
    Serial.println(WiFi.softAPIP());

    server.begin();
    Serial.println("TCP server started");
  }

  void loop() {
    this->handleNewSlaveJoin();
    this->recvDataFromSlaves();
    if (wifiTimer.autoExpire(1000)) {
      this->sendDataToSlaves();
    }
  }

  void handleNewSlaveJoin() {
    WiFiClient newClient = server.available();

    if (newClient) {
      Serial.println("New slave trying to connect...");

      for (int i = 0; i < MAX_SLAVES; i++) {
        if (!slaveClients[i] || !slaveClients[i].connected()) {
          slaveClients[i] = newClient;
          Serial.print("Slave connected at slot ");
          Serial.print(i);
          Serial.print(" - IP: ");
          Serial.print(slaveClients[i].remoteIP());
          Serial.print(", Port: ");
          Serial.println(slaveClients[i].remotePort());
          break;
        }
      }
    }
  }

  void recvDataFromSlaves() {
    for (int i = 0; i < MAX_SLAVES; i++) {
      if (slaveClients[i] && slaveClients[i].connected()) {
        if (slaveClients[i].available()) {
          String data = slaveClients[i].readStringUntil('\n');
          data.trim();
          databaseBroker.setSlaveResponse(i, data);
          Serial.print("Received from slave IP: ");
          Serial.print(slaveClients[i].remoteIP());
          Serial.print(", Port: ");
          Serial.print(slaveClients[i].remotePort());
          Serial.print(" - Data: ");
          Serial.print(data);
        }
      } else {
        // Clean up disconnected slot
        if (slaveClients[i]) {
          slaveClients[i].stop();
        }
      }
    }
  }

  void sendDataToSlaves() {
    for (int i = 0; i < MAX_SLAVES; i++) {
      if (slaveClients[i] && slaveClients[i].connected()) {
        // slaveControlDatabase[i].set(1, "IFCU-1", 1, 1, 1, 2500, 2700, 1500, 2700);
        // slaveClients[i].println(slaveControlDatabase[i].dataStr);
      } else {
        if (slaveClients[i]) {
          slaveClients[i].stop();
        }
      }
    }
  }

  ~WiFiBroker(){};
};

extern WiFiBroker wifiBroker;

#endif