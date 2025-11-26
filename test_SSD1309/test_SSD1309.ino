#include "WiFi.h"

// Replace with your network details

const char* ssid = "REC Guest - 16F";
const char* password = "guest@@2022";

IPAddress staticIP(192, 168, 1, 184);
IPAddress gateway(10, 236, 208, 1);
IPAddress subnet(255, 255, 254, 0);

void setup() {
  Serial.begin(115200);

  // Configure static IP
  if (!WiFi.config(staticIP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Your code here
}
