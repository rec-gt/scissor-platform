#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#define EEPROM_SIZE 1024

const char* ssid = "REC Guest";        // Enter SSID here
const char* password = "guest@@2022";  // Enter Password here

WebServer server(80);

uint16_t hrDatabase[32][8];
uint16_t irDatabase[32][8];

void writeEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  int address = 0;

  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 8; j++) {
      EEPROM.put(address, hrDatabase[i][j]);
      address += sizeof(uint16_t);
    }
  }
  EEPROM.commit();
}

void initDB() {
  for (size_t i = 0; i < 32; i++) {
    hrDatabase[i][0] = 2500;
  }
  for (size_t i = 0; i < 32; i++) {
    irDatabase[i][0] = 2500;
  }
}

int counter = 0;

void setup() {
  Serial.begin(115200);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.on("/fetch_all", handleFetchAll);
  server.on("/set_target", HTTP_POST, handleSetTarget);

  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  initDB();
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  counter++;
  server.send(200, "text/html", createHTML());
}

void handleSetTarget() {
  int id;

  if (server.hasArg("id")) {
    id = server.arg("id").toInt();
  }

  if (server.hasArg("setTempIncrease")) {
    hrDatabase[id][1] += 50;
  }

  if (server.hasArg("setTempDecrease")) {
    hrDatabase[id][1] -= 50;
  }

  if (server.hasArg("mode=0")) {
    hrDatabase[id][2] = 0;
  }

  if (server.hasArg("mode=1")) {
    hrDatabase[id][2] = 1;
  }

  if (server.hasArg("mode=2")) {
    hrDatabase[id][2] = 2;
  }

  if (server.hasArg("speed=0")) {
    hrDatabase[id][3] = 0;
  }

  if (server.hasArg("speed=1")) {
    hrDatabase[id][3] = 1;
  }

  if (server.hasArg("speed=2")) {
    hrDatabase[id][3] = 2;
  }

  Serial.print(id);
  Serial.print(", ");
  Serial.print(hrDatabase[id][0]);
  Serial.println();

  writeEEPROM();
}

void handleFetchAll() {
  String str = "";

  str += "{";
  str += "HR:[";
  for (size_t i = 0; i < 32; i++) {
    str += "[";
    for (size_t j = 0; j < 8; j++) {
      str += hrDatabase[i][j];
      str += ",";
    }
    str += "],";
  }
  str += "],";
  str += "IR:[";
  for (size_t i = 0; i < 32; i++) {
    str += "[";
    for (size_t j = 0; j < 8; j++) {
      str += irDatabase[i][j];
      str += ",";
    }
    str += "],";
  }
  str += "],";
  str += "}";


  server.send(200, "text/plain", str);
  Serial.println(str);
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String createHTML() {
  String str = "<!DOCTYPE html> <html>";
  str += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">";
  str += "<style>";
  str += "body {font-family: Arial, sans-serif; color: #444; text-align: center;}";
  str += ".title {font-size: 30px; font-weight: bold; letter-spacing: 2px; margin: 80px 0 55px;}";
  str += ".counter {font-size: 80px; font-weight: 300; line-height: 1; margin: 0px; color: #4285f4;}";
  str += "</style>";
  str += "</head>";
  str += "<body>";
  str += "<h1 class=\"title\">VISITOR COUNTER</h1>";
  str += "<div class=\"counter\">";
  str += counter;
  str += "</div>";
  str += "</body>";
  str += "</html>";
  return str;
}