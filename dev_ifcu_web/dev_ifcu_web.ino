
#include <WiFi.h>
#include <WiFiClient.h>
#include "SerialRecv.h"
#include "Globals.h"

const char* ssid = "ifcu_16f_test";
const char* password = "never_gonna_give_you_up";

WiFiServer server(80);

IPAddress LocalIP(192, 168, 1, 1);
IPAddress Gateway(192, 168, 1, 1);
IPAddress SubNet(255, 255, 255, 0);

SerialRecv serialRecv;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  WiFi.mode(WIFI_AP);

  WiFi.softAPConfig(LocalIP, Gateway, SubNet);
  WiFi.softAP(ssid, password);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  server.begin();
}

void loop() {
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

    // Serial.println("Request: " + request);

    // Serve the HTML page
    if (request.indexOf("GET / ") >= 0) {
      client.print("HTTP/1.1 200 OK\r\n");
      client.print("Content-Type: text/html\r\n");
      client.print("Connection: close\r\n\r\n");
      client.print(R"rawliteral(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>iFCU Control</title>
    <style>
      * {
        font-family: sans-serif;
        box-sizing: border-box;
        color: #444;
      }

      html,
      body {
        width: 100%;
        margin: 0;
        padding: 0;
        background: #a5dfff;
      }

      .flex {
        display: flex;
        justify-content: space-between;
        align-items: center;
      }

      .w-full {
        width: 100%;
      }

      .center {
        width: 100%;
        display: flex;
        justify-content: center;
        align-items: center;
        text-align: center;
      }

      option {
        background-color: #f9f9f9;
        color: #333;
        padding: 8px 12px;
      }

      .panel {
        display: flex;
        flex-direction: column;
        gap: 1rem;
        width: 100%;
        max-width: 400px;
        border-radius: 10px;
        border: 2px #444 solid;
        background-color: #fff;
        padding: 1rem;
      }

      .bar {
        background-color: #f9f9f9;
        border-radius: 10px;
        padding: 0.5rem 1rem;
      }

      #on-off {
        display: flex;
        gap: 5px;
      }

      .on-off,
      .on-off-active {
        background-color: #ccc;
        color: #fff;
        border-radius: 5px;
        padding: 2.5px 5px;
        font-size: 12pt;
        cursor: pointer;
      }

      .on-off-active {
        background-color: #a5dfff;
      }

      .room-temp {
        width: 50%;
        height: 50px;
        font-size: 24pt;
      }

      .switch {
        width: 50px;
        cursor: pointer;
      }

      #mode,
      #fan-speed {
        text-align: center;
        width: 120px;
        padding: 8px 12px;
        border: 1px solid #ccc;
        border-radius: 4px;
        background-color: #f9f9f9;
        color: #333;
        font-size: 16px;
        cursor: pointer;
      }
    </style>
    <script>
      let onOff = 0;
      let roomTemp = 2400;
      let spt = 2500;
      let mode = 0;
      let fanSpeed = 0;

      async function sendCMD(c) {
        try {
          const response = await fetch(`/cmd?c=${c}`);
        } catch (error) {
          console.error("Error in cmd:", error);
        }
      }

      function rerenderData() {
        document.getElementById("on-off").innerHTML = [
          `<div class="on-off" onclick="sendCMD(1)">ON</div>
              <div class="on-off-active" onclick="sendCMD(0)">OFF</div>`,
          `<div class="on-off-active" onclick="sendCMD(1)">ON</div>
              <div class="on-off" onclick="sendCMD(0)">OFF</div>`,
        ][onOff];
        document.getElementById("room-temp").textContent = roomTemp;
        document.getElementById("set-point").textContent = spt;
        document.getElementById("mode").value = mode;
        document.getElementById("fan-speed").value = fanSpeed;
      }

      async function setSPT(value) {
        spt += value;
        try {
          const response = await fetch(`/spt?v=${spt}`);
          rerenderData();
        } catch (error) {
          console.error("Error in cmd:", error);
        }
      }

      async function setMode() {
        let c = [2, 3, 4][parseInt(document.getElementById("mode").value)];
        await sendCMD(c);
      }

      async function setFanSpeed() {
        let c = [7, 8, 9][parseInt(document.getElementById("fan-speed").value)];
        await sendCMD(c);
      }

      async function fetchData() {
        try {
          const response = await fetch("/data");
          let data = JSON.parse(await response.text());
          //   data = [1, 22, 22, 0, 1];

          onOff = data[0];
          roomTemp = data[1];
          spt = data[2];
          mode = data[3];
          fanSpeed = data[4];

          rerenderData();
        } catch (error) {
          console.error("Error fetching data:", error);
        }
      }

      setInterval(() => {
        fetchData();
      }, 1000);
    </script>
  </head>

  <body>
    <div style="padding: 1rem">
      <div class="panel">
        <div class="flex bar">
          <div>FCU-1F-01</div>
          <div class="flex" style="gap: 1rem">
            <div id="on-off">
              <div class="on-off-active" onclick="sendCMD(1)">ON</div>
              <div class="on-off" onclick="sendCMD(0)">OFF</div>
            </div>
          </div>
        </div>

        <div>
          <div class="flex">
            <div class="center">Room Temp.</div>
            <div class="center">Set Point</div>
          </div>
          <div class="flex">
            <div class="center">
              <div class="center room-temp">
                <div id="room-temp">22.2</div>
                <div>°C</div>
              </div>
            </div>
            <div class="center" style="justify-content: space-between">
              <div class="switch" onclick="setSPT(-50)">◀</div>
              <div id="set-point">25.5</div>
              <div>°C</div>
              <div class="switch" onclick="setSPT(50)">▶</div>
            </div>
          </div>
        </div>

        <div>
          <div class="flex">
            <div class="center">Mode</div>
            <div class="center">Fan Speed</div>
          </div>
          <div class="flex">
            <div class="center">
              <div class="center" style="height: 50px">
                <select id="mode" onchange="setMode()">
                  <option value="0">Auto</option>
                  <option value="1">Manual</option>
                  <option value="2">Fan</option>
                </select>
              </div>
            </div>

            <div class="center">
              <div class="center" style="height: 50px">
                <select id="fan-speed" onchange="setFanSpeed()">
                  <option value="0">Low</option>
                  <option value="1">Medium</option>
                  <option value="2">High</option>
                </select>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </body>
</html>

)rawliteral");
    }

    // Serve the data endpoint
    else if (request.indexOf("GET /data") >= 0) {

      client.print("HTTP/1.1 200 OK\r\n");
      client.print("Content-Type: text/plain\r\n");
      client.print("Connection: close\r\n\r\n");
      String response = "";
      response += "[";
      response += String(serialRecv.values[0]);
      response += ",";
      response += String(serialRecv.values[1] / 100.0, 1);
      response += ",";
      response += String(serialRecv.values[2] / 100.0, 1);
      response += ",";
      response += String(serialRecv.values[3]);
      response += ",";
      response += String(serialRecv.values[4]);
      response += "]";
      client.print(response);
    }

    else if (request.indexOf("GET /cmd") >= 0) {
      if (request.indexOf("/cmd?c=") >= 0) {
        int startIndex = request.indexOf("/cmd?c=") + 7;
        String cmd = request.substring(startIndex, startIndex + 2);
        String sendBuffer = "CMD:";
        sendBuffer += cmd;
        Serial2.println(sendBuffer);
        Serial.println(sendBuffer);
        client.print("OK");
      }
    }

    else if (request.indexOf("GET /spt") >= 0) {
      if (request.indexOf("/spt?v=") >= 0) {
        int startIndex = request.indexOf("/spt?v=") + 7;
        String cmd = request.substring(startIndex, startIndex + 4);
        String sendBuffer = "CMD:";
        sendBuffer += cmd;
        Serial2.println(sendBuffer);
        Serial.println(sendBuffer);
        client.print("OK");
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
  }
}