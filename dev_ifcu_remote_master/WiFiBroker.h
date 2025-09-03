#include <WiFi.h>

#ifndef WiFiBroker_H
#define WiFiBroker_H

#define MAX_SLAVES 5
#define SERVER_PORT 80

const char* apSSID = "ifcu-remote-16f-test";
const char* apPassword = "ifcu-remote-16f-test";

IPAddress staticIP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(SERVER_PORT);
WiFiClient slaveClients[MAX_SLAVES];

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
    this->handleNewConnection();
  }

  void handleNewConnection() {
    WiFiClient client = server.available();
    if (client) {
      Serial.println("New client connected");

      Serial.print("Client IP address: ");
      Serial.println(client.remoteIP());

      client.stop();
    }
  }

  ~WiFiBroker(){};
};

extern WiFiBroker wifiBroker;

#endif