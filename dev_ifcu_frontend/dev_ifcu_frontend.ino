#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ASUS_A6";
const char* password = "n5npy#6gfe";

// Create a web server on port 80
WebServer server(80);

// HTML content to display on the webpage
const char webpage[] PROGMEM = R"rawliteral(

<!doctype html>
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
        user-select: none;
      }

      html,
      body {
        width: 100%;
        margin: 0;
        padding: 0;
        background: #a5dfff;
      }

      #parent {
        display: flex;
        gap: 1rem;
        flex-wrap: wrap;
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
        border: 2px #666 solid;
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

      .arrow {
        border: solid black;
        border-width: 0 3px 3px 0;
        display: inline-block;
        padding: 3px;
      }

      .arrow-right {
        transform: rotate(-45deg);
        -webkit-transform: rotate(-45deg);
      }

      .arrow-left {
        transform: rotate(135deg);
        -webkit-transform: rotate(135deg);
      }

      .mode,
      .fan-speed {
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

      .rotate-svg {
        animation: rotation 2s linear infinite;
        transform-origin: center;
        will-change: transform;
      }

      @keyframes rotation {
        from {
          transform: rotate(0deg);
        }
        to {
          transform: rotate(360deg);
        }
      }

      .sync-fail {
        fill: red;
      }
    </style>
  </head>

  <body>
    <div style="padding: 1rem">
      <h1>RGT iFCU Remote Control Panel</h1>
      <div id="parent"></div>
    </div>
  </body>
  <script>
    const database = [
      {
        id: 'ifcu-001',
        name: 'IFCU-001',
        ip: '192.168.50.101',
      },
      {
        id: 'ifcu-002',
        name: 'IFCU-002',
        ip: '192.168.50.102',
      },
      {
        id: 'ifcu-003',
        name: 'IFCU-003',
        ip: '192.168.50.103',
      },
    ];
  </script>

  <script>
    let renderLock = false;

    async function setCmd(id, cmd) {
      const idx = database.findIndex((e) => e.id === id);
      if (idx > -1) {
        renderLock = true;

        const device = database[idx];
        const { id, ip } = device;
        fetch(`http://${ip}/set?cmd=${cmd}`);

        if (device) {
          if (cmd === 'A') {
            device.onOff = '1';
          } else if (cmd === 'B') {
            device.onOff = '0';
          } else if (cmd === 'C') {
            device.mode = '0';
          } else if (cmd === 'D') {
            device.mode = '1';
          } else if (cmd === 'E') {
            device.mode = '2';
          } else if (cmd === 'F') {
            device.speed = '0';
          } else if (cmd === 'G') {
            device.speed = '1';
          } else if (cmd === 'H') {
            device.speed = '2';
          } else if (cmd === 'I') {
            device.setTemp = (parseInt(device.setTemp) + 50).toString();
          } else if (cmd === 'J') {
            device.setTemp = (parseInt(device.setTemp) - 50).toString();
          }
        }

        Object.assign(device, { isSynced: '0' });
        renderData(id);

        renderLock = false;
      }
    }

    setInterval(() => {
      fetchAll();
    }, 3000);
  </script>

  <script>
    async function fetchAll() {
      try {
        for (const device of database) {
          const { id, ip } = device;

          await fetch(`http://${ip}/get`, {
            signal: AbortSignal.timeout(10000),
          })
            .then(async (res) => {
              const jsonData = await res.json();
              return jsonData;
            })
            .then((data) => {
              if (renderLock) {
                return;
              }

              Object.assign(device, {
                isConnected: true,
              });

              if (data.isSynced === '1' && data.queueLen === '0') {
                Object.assign(device, { ...data });
              } else {
                Object.assign(device, {
                  isSynced: data.isSynced,
                  queueLen: data.queueLen,
                });
              }

              if (device.modbusComm !== '1') {
                Object.assign(device, { isConnected: false });
              }
            })
            .catch((err) => {
              Object.assign(device, {
                isConnected: false,
              });
            })
            .finally(() => {
              renderData(id);
            });
        }
      } catch (error) {
        console.error('Error in cmd:', error);
      }
    }
  </script>

  <script>
    function preRender() {
      const parent = document.getElementById('parent');
      parent.innerHTML = '';

      for (const data of database) {
        const { id, name } = data;
        const header = `
              <div class="flex bar">
                <div>${name}</div>
                <div class="flex" style="gap: 1rem">
                    <svg id="${id}-sync" class="rotate-svg" width="20px" height="20px" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg" fill="#00FF00"><path fill-rule="evenodd" clip-rule="evenodd" d="M2.006 8.267L.78 9.5 0 8.73l2.09-2.07.76.01 2.09 2.12-.76.76-1.167-1.18a5 5 0 0 0 9.4 1.983l.813.597a6 6 0 0 1-11.22-2.683zm10.99-.466L11.76 6.55l-.76.76 2.09 2.11.76.01 2.09-2.07-.75-.76-1.194 1.18a6 6 0 0 0-11.11-2.92l.81.594a5 5 0 0 1 9.3 2.346z"/></svg>
                    <div id="${id}-on" class="on-off" onclick="setCmd('${id}','A')">ON</div>
                    <div id="${id}-off" class="on-off" onclick="setCmd('${id}','B')">OFF</div>
                </div>
              </div>
            `;

        const body = `
            <div class="panel">
              ${header}
              <div>
                <div class="flex">
                  <div class="center">Room Temp.</div>
                  <div class="center">Set Point</div>
                </div>
                <div class="flex">
                  <div class="center">
                    <div class="center room-temp">
                      <div id="${id}-roomTemp"></div>
                      <div>°C</div>
                    </div>
                  </div>
                  <div class="center" style="justify-content: space-between">
                    <div class="switch" onclick="setCmd('${id}','J')"><i class="arrow arrow-left"></i></div>
                    <div id="${id}-setTemp"></div>
                    <div>°C</div>
                    <div class="switch" onclick="setCmd('${id}','I')"><i class="arrow arrow-right"></i></div>
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
                      <select id="${id}-mode" class="mode" onchange="setCmd('${id}',this.value)">
                        <option disabled hidden value="-">-</option>
                        <option value="C">Auto</option>
                        <option value="D">Manual</option>
                        <option value="E">Fan</option>
                      </select>
                    </div>
                  </div>

                  <div class="center">
                    <div class="center" style="height: 50px">
                      <select id="${id}-speed" class="fan-speed" onchange="setCmd('${id}',this.value)">
                        <option disabled hidden value="-">-</option>
                        <option value="F">Low</option>
                        <option value="G">Medium</option>
                        <option value="H">High</option>
                      </select>
                    </div>
                  </div>
                </div>
              </div>
            </div>
            `;

        parent.innerHTML += body;
      }
    }

    function renderData(id) {
      const idx = database.findIndex((e) => e.id === id);

      if (idx > -1) {
        const {
          onOff,
          mode,
          speed,
          setTemp,
          roomTemp,
          isConnected,
          isSynced,
          queueLen,
        } = database[idx];

        if (isConnected) {
          if (onOff === '1') {
            document.getElementById(`${id}-on`).classList.add('on-off-active');
            document
              .getElementById(`${id}-off`)
              .classList.remove('on-off-active');
          }

          if (onOff === '0') {
            document
              .getElementById(`${id}-on`)
              .classList.remove('on-off-active');
            document.getElementById(`${id}-off`).classList.add('on-off-active');
          }

          document.getElementById(`${id}-roomTemp`).innerHTML = (
            parseInt(roomTemp) / 100
          ).toFixed(1);

          document.getElementById(`${id}-setTemp`).innerHTML = (
            parseInt(setTemp) / 100
          ).toFixed(1);

          document.getElementById(`${id}-mode`).value = ['C', 'D', 'E'][mode];
          document.getElementById(`${id}-speed`).value = ['F', 'G', 'H'][speed];

          if (document.getElementById(`${id}-sync`)) {
            document.getElementById(`${id}-sync`).classList.remove('sync-fail');
          }
          if (isSynced === '0' || queueLen !== '0') {
            if (document.getElementById(`${id}-sync`)) {
              document.getElementById(`${id}-sync`).style.display = 'block';
            }
          } else {
            if (document.getElementById(`${id}-sync`)) {
              document.getElementById(`${id}-sync`).style.display = 'none';
            }
          }
        } else {
          if (document.getElementById(`${id}-sync`)) {
            document.getElementById(`${id}-sync`).classList.add('sync-fail');
            document.getElementById(`${id}-sync`).style.display = 'block';
          }
          document.getElementById(`${id}-on`).classList.remove('on-off-active');
          document
            .getElementById(`${id}-off`)
            .classList.remove('on-off-active');
          document.getElementById(`${id}-roomTemp`).innerHTML = '--.-';
          document.getElementById(`${id}-setTemp`).innerHTML = '--.-';
          document.getElementById(`${id}-mode`).value = '-';
          document.getElementById(`${id}-speed`).value = '-';
        }
      }
    }
  </script>
  <script>
    preRender();
  </script>
</html>

)rawliteral";

void handleRoot() {
  // Send the HTML content to the client
  server.send(200, "text/html", webpage);
}

void setup() {
  // Start Serial Communication
  Serial.begin(115200);
  Serial.println();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure the web server
  server.on("/", handleRoot); // Serve the HTML page at the root URL
  server.begin(); // Start the server
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient(); // Handle incoming client requests
}