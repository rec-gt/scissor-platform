#include <WiFi.h>

const char* apSSID = "iFCU-16f-test-ckyt";    // Master's AP SSID
const char* apPassword = "iFCU-16f-test-ckyt";    // Master's AP password (min 8 chars)
#define MAX_SLAVES 5                    // Max number of slave connections
#define SERVER_PORT 80                  // TCP port to listen on

// Static IP configuration for AP
IPAddress staticIP(192, 168, 1, 100);     // Master's static IP (default for ESP32 AP)
IPAddress gateway(192, 168, 1, 1);      // Gateway (same as AP IP)
IPAddress subnet(255, 255, 255, 0);     // Subnet mask

WiFiServer server(SERVER_PORT);         // TCP server
WiFiClient slaveClients[MAX_SLAVES];    // Array to hold connected slaves

void setup() {
  Serial.begin(115200);

  // Configure static IP for AP
  WiFi.mode(WIFI_AP);                   // Set to Access Point mode
  if (!WiFi.softAPConfig(staticIP, gateway, subnet)) {
    Serial.println("Failed to configure AP static IP");
  }

  // Start the AP
  WiFi.softAP(apSSID, apPassword);
  Serial.println("Access Point started");
  Serial.print("Master AP IP: ");
  Serial.println(WiFi.softAPIP()); // Should print 192.168.4.1

  // Start TCP server
  server.begin();
  Serial.println("TCP server started");
}

void loop() {
  // Check for new slave connections
  WiFiClient newClient = server.available();
  if (newClient) {
    Serial.println("New slave trying to connect...");
    // Find a free slot
    for (int i = 0; i < MAX_SLAVES; i++) {
      if (!slaveClients[i] || !slaveClients[i].connected()) {
        slaveClients[i] = newClient;
        Serial.print("Slave connected at slot ");
        Serial.print(i);
        Serial.print(" - IP: ");
        Serial.print(slaveClients[i].remoteIP()); // e.g., 192.168.4.2
        Serial.print(", Port: ");
        Serial.println(slaveClients[i].remotePort());
        break;
      }
    }
  }

  // Check for data from each connected slave
  for (int i = 0; i < MAX_SLAVES; i++) {
    if (slaveClients[i] && slaveClients[i].connected()) {
      if (slaveClients[i].available()) {
        // Read data from slave
        String data = slaveClients[i].readStringUntil('\n');
        data.trim();
        Serial.print("Received from slave IP: ");
        Serial.print(slaveClients[i].remoteIP());
        Serial.print(", Port: ");
        Serial.print(slaveClients[i].remotePort());
        Serial.print(" - Data: ");
        Serial.println(data);

        // Optional: Send a response back to this specific slave
        // slaveClients[i].println("Ack from master: " + data);
      }
    } else {
      // Clean up disconnected slot
      if (slaveClients[i]) {
        slaveClients[i].stop();
      }
    }
  }

  // Example: Send targeted data to a specific slave (replace with your logic)
  IPAddress targetIP(192, 168, 4, 2); // Replace with target slave IP (e.g., from Serial)
  uint16_t targetPort = 80;        // Replace with target slave port (from Serial)
  // sendToSpecificSlave(targetIP, targetPort, "Command: Reset");

  delay(10); // Small delay to avoid CPU overload
}

// Function to send data to a specific slave by IP and port
void sendToSpecificSlave(IPAddress targetIP, uint16_t targetPort, String message) {
  for (int i = 0; i < MAX_SLAVES; i++) {
    if (slaveClients[i] && slaveClients[i].connected() &&
        slaveClients[i].remoteIP() == targetIP &&
        slaveClients[i].remotePort() == targetPort) {
      slaveClients[i].println(message);
      Serial.print("Sent to slave IP: ");
      Serial.print(targetIP);
      Serial.print(", Port: ");
      Serial.print(targetPort);
      Serial.print(" - Message: ");
      Serial.println(message);
      return;
    }
  }
  Serial.println("Target slave not found or disconnected.");
}