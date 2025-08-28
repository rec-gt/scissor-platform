
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

const char* ssid = "REC Guest";
const char* password = "guest@@2022";

const char* serverName = "http://10.236.208.127:3010";
String url = String(serverName) + "/f-l/1";

// TODO: ASK FOR STATIC IP

WebServer server(80);

unsigned long prevMillis = millis();

void handleGetData() {
  server.send(200, "application/json", "2500,2500,2,2,1700,3000");
}

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("IP Address: " + WiFi.localIP().toString());
  server.on("/getData", handleGetData);
  server.begin();
  Serial.println("HTTP server started!");
}

void loop() {
  if (millis() - prevMillis > 3000) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(url);
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        String response = http.getString();
        Serial.println(response);
      } else {
        Serial.printf("Error code: %d\n", httpResponseCode);
      }

      http.end();

    } else {
      Serial.println("WiFi Disconnected");
    }

    prevMillis = millis();
  }


  server.handleClient();

  delay(3000);
}
