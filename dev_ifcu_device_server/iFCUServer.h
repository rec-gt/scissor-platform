#ifndef IFCU_SERVER_H
#define IFCU_SERVER_H

#include <WebServer.h>

WebServer server(80);

void handleSet() {
  if (server.hasArg("cmd")) {
    String cmd = server.arg("cmd");
    QUEUE += cmd;
    Serial.print("Queue received: ");
    Serial.println(QUEUE);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing parameters.");
  }
}

void handleGet() {
  jsonStr = "{";

  jsonStr += '\"';
  jsonStr += "onOff";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[0];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "mode";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[1];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "speed";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[2];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "setTemp";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[3];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "roomTemp";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += WRITE_DATA[4];
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "isSynced";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += isSynced;
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "modbusComm";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += ifcuModbus.mbSuccess();
  jsonStr += '\"';
  jsonStr += ",";

  jsonStr += '\"';
  jsonStr += "queueLen";
  jsonStr += '\"';
  jsonStr += ":";

  jsonStr += '\"';
  jsonStr += QUEUE.length();
  jsonStr += '\"';

  jsonStr += "}";

  server.send(200, "application/json", jsonStr);
}

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
      fetch(`http://192.168.1.1/wifi/set?ssid=${ssid}&password=${password}`, {
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

  server.send(200, "text/html", settingPage);
}

void handleWiFiSet() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    memory.setStr(WIFI_SSID_ADDRESS, WIFI_SSID_LEN, ssid);
    memory.setStr(WIFI_PASSWORD_ADDRESS, WIFI_PASSWORD_LEN, password);

    memory.getStr(WIFI_SSID_ADDRESS, WIFI_SSID_LEN, WIFI_SSID);
    memory.getStr(WIFI_PASSWORD_ADDRESS, WIFI_PASSWORD_LEN, WIFI_PASSWORD);

    Serial.println(WIFI_SSID);
    Serial.println(WIFI_PASSWORD);

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing parameters.");
  }
}

class iFCUServer {

private:
  void restartWebServer() {
    Serial.println("Restarting web server...");
    server.stop();
    Serial.println("Web server stopped.");
    delay(1000);
    server.enableCORS();
    server.on("/set", HTTP_GET, handleSet);
    server.on("/get", HTTP_GET, handleGet);
    server.on("/setting", HTTP_GET, handleSettingPage);
    // http://192.168.1.1/wifi/set?ssid=123123&password=123123
    server.on("/wifi/set", HTTP_GET, handleWiFiSet);
    server.begin();
    Serial.println("Web server started!");
  }

public:
  void loop() {
    if (restartServerFlag) {
      restartWebServer();
      restartServerFlag = false;
    }

    server.handleClient();
  }
};

extern iFCUServer ifcuServer;

#endif
