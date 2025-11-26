
#include <WiFi.h>
#include <WiFiClient.h>
#include "WiFiConn.h"
#include "SerialRecv.h"
#include "Globals.h"
#include "iFCUModbus.h"

iFCUModbus ifcuModbus;

WiFiConn wifiConn;

WiFiServer server(80);

SerialRecv serialRecv;

void setup() {
  wifiConn.init();

  Serial.begin(115200);

  server.begin();

  ifcuModbus.init();
}

void loop() {
  ifcuModbus.loop();

  serialRecv.listen();

  /* ========== Web Server ========= */

  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected!");
    String request = "";

    // Read the request
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        // Break when request ends
        if (request.endsWith("\r\n\r\n")) {
          break;
        }
      }
    }


    // Serve the data endpoint
    if (request.indexOf("GET /set") >= 0) {
      client.print("HTTP/1.1 200 OK\r\n");
      client.print("Content-Type: text/plain\r\n");
      client.print("Connection: close\r\n\r\n");
      String response = "";
      response += "[";
      response += String(serialRecv.values[0]);
      response += ",";
      response += String(serialRecv.values[1]);
      response += ",";
      response += String(serialRecv.values[2]);
      response += ",";
      response += String(serialRecv.values[3]);
      response += ",";
      response += String(serialRecv.values[4]);
      response += "]";
      client.print(response);
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
  }
}