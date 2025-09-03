#include <WiFi.h>

const char* apSSID = "ifcu-remote-16f-test";
const char* apPassword = "ifcu-remote-16f-test";

IPAddress staticIP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

void setup() {
  Serial.begin(9600);

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
  WiFiClient newClient = server.available();

  if (newClient) {
    Serial.println("New slave trying to connect...");
    Serial.print(" - IP: ");
    Serial.print(newClient.remoteIP());
    Serial.print(", Port: ");
    Serial.println(newClient.remotePort());
  }
  delay(100);
}