#ifndef BACKEND_SERVER_H
#define BACKEND_SERVER_H

#include "./Globals.h"

WebServer server2(81);

void handleSettingPage() {
  const char settingPage[] PROGMEM = R"rawliteral(

 <!doctype html>
<html>
  <body>
    <input type="text" id="ssid" name="input1" placeholder="New SSID" />
    <br />
    <br />
    <input type="text" id="password" name="input2" placeholder="New Password" />
    <br />
    <br />
    <button onclick="submit()">Submit</button>
    <br />
    <br />
    <div id="result"></div>
  </body>
  <script>
    const submit = () => {
      const ssid = document.getElementById("ssid").value;
      const password = document.getElementById("password").value;
      fetch(`http://192.168.1.1:81/wifi/set?ssid=${ssid}&password=${password}`, {
        method: "GET",
      })
        .then(() => {
          document.getElementById("result").innerHTML = "OK";
        })
        .catch(() => {
          document.getElementById("result").innerHTML = "FAIL";
        });
    };
  </script>
</html>

)rawliteral";

  server2.send(200, "text/html", settingPage);
}

void handleWiFiSet() {
  if (server2.hasArg("ssid") && server2.hasArg("password")) {
    String ssid = server2.arg("ssid");
    String password = server2.arg("password");

    memory.writeStr(WIFI_SSID_ADDRESS, WIFI_SSID_LEN, ssid);
    memory.writeStr(WIFI_PASSWORD_ADDRESS, WIFI_PASSWORD_LEN, password);

    memory.readStr(WIFI_SSID_ADDRESS, WIFI_SSID_LEN, WIFI_SSID);
    memory.readStr(WIFI_PASSWORD_ADDRESS, WIFI_PASSWORD_LEN, WIFI_PASSWORD);

    Serial.println(WIFI_SSID);
    Serial.println(WIFI_PASSWORD);

    server2.send(200, "text/plain", "OK");
  } else {
    server2.send(400, "text/plain", "Missing parameters.");
  }
}

class BackendServer {
public:
  void init() {
    server2.enableCORS();
    server2.on("/setting", HTTP_GET, handleSettingPage);
    server2.on("/wifi/set", HTTP_GET, handleWiFiSet);
    server2.begin();
  }

  void loop() {
    server2.handleClient();
  }
};

extern BackendServer backendServer;

#endif
