
#include <WiFi.h>
#include <WiFiClient.h>

const char* ssid = "ifcu_16f_test";
const char* password = "never_gonna_give_you_up";

WiFiServer server(80);

IPAddress LocalIP(192, 168, 1, 1);
IPAddress Gateway(192, 168, 1, 1);
IPAddress SubNet(255, 255, 255, 0);

String serialRecv = "";
String values[5];
int valueIdx = 0;

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

void splitString(String str, size_t size) {
  String values[size];
  int valueIdx = 0;
}

void loop() {
  if (Serial2.available() >= 7) {
    uint8_t data[7];

    Serial2.readBytes(data, 7);

    uint8_t int1 = data[0];
    uint16_t int2 = data[1] | (data[2] << 8);
    uint16_t int3 = data[3] | (data[4] << 8);
    uint8_t int4 = data[5];
    uint8_t int5 = data[6];

    Serial.print("int1: ");
    Serial.println(int1);
    Serial.print("int2: ");
    Serial.println(int2);
    Serial.print("int3: ");
    Serial.println(int3);
    Serial.print("int4: ");
    Serial.println(int4);
    Serial.print("int5: ");
    Serial.println(int5);

    // while (Serial.read() >= 0) {};
  }

  // if (Serial2.available()) {
  //   while (Serial2.available()) {
  //     char c = Serial2.read();

  //     if (c != '\r' && c != '\n') {
  //       serialRecv += c;
  //     }

  //     if (c == '\r') {
  //       while (serialRecv.length() > 0) {
  //         int commaIndex = serialRecv.indexOf(",");
  //         String valueStr = serialRecv.substring(0, commaIndex);

  //         values[valueIdx] = valueStr;
  //         valueIdx++;

  //         if (commaIndex == -1) break;

  //         serialRecv = serialRecv.substring(commaIndex + 1);
  //       }

  //       values[1] = String(values[1].toInt() / 100.0, 1);
  //       values[2] = String(values[2].toInt() / 100.0, 1);

  //       valueIdx = 0;
  //       serialRecv = "";
  //     }
  //   }
  // }

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
      .on-off,
      .on-off-on,
      .on-off-off {
        background-color: #a5dfff;
        color: #fff;
        border-radius: 5px;
        padding: 2.5px 5px;
        font-size: 12pt;
      }
      .on-off-off {
        background-color: #ccc;
      }
      .room-temp {
        width: 50%;
        height: 50px;
        font-size: 24pt;
      }
      .switch {
        width: 50px;
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
      }
    </style>
    <script>
      async function sendCMD(c) {
        try {
          const response = await fetch(`/cmd?c=${c}`);
          const data = await response.text();
          if (data) {
            alert("Ok");
          }
        } catch (error) {
          console.error("Error in cmd:", error);
        }
      }

      async function fetchData() {
        try {
          const response = await fetch("/data");
          let data = JSON.parse(await response.text());
        //   data = [1, 22, 22, 0, 1];
          document.getElementById("on-off").innerHTML = [
            `<div class="on-off-off" onclick="sendCMD(1)">OFF</div>`,
            `<div class="on-off-on" onclick="sendCMD(0)">ON</div>`
          ][data[0]];
          document.getElementById("room-temp").textContent = data[1];
          document.getElementById("set-point").textContent = data[2];
          document.getElementById("mode").value = data[3];
          document.getElementById("fan-speed").value = data[4];
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
              <div class="on-off" onclick="sendCMD(1)">ON</div>
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
              <div class="switch" onclick="sendCMD(2)">◀</div>
              <div id="set-point">25.5</div>
              <div>°C</div>
              <div class="switch" onclick="sendCMD(3)">▶</div>
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
                <select id="mode" onchange="sendCMD(4)">
                  <option value="0">Auto</option>
                  <option value="1">Manual</option>
                  <option value="2">Fan</option>
                </select>
              </div>
            </div>

            <div class="center">
              <div class="center" style="height: 50px">
                <select id="fan-speed" onchange="sendCMD(5)">
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
      response += values[0];
      response += ",";
      response += values[1];
      response += ",";
      response += values[2];
      response += ",";
      response += values[3];
      response += ",";
      response += values[4];
      response += "]";
      client.print(response);
    }

    else if (request.indexOf("GET /cmd") >= 0) {
      if (request.indexOf("/cmd?c=") >= 0) {
        int startIndex = request.indexOf("/cmd?c=") + 7;
        String cmd = request.substring(startIndex, startIndex + 1);

        client.print("OK");

        if (cmd == "0") {
          Serial.println("cmd 1 received!");
        } else if (cmd == "1") {
          Serial.println("cmd 1 received!");
        } else if (cmd == "2") {
          Serial.println("cmd 2 received!");
        } else if (cmd == "3") {
          Serial.println("cmd 3 received!");
        } else {
          Serial.println("Unknown cmd received!");
        }
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
  }
}