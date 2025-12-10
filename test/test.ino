#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#define EEPROM_SIZE 1024

const char* ssid = "REC Guest";        // Enter SSID here
const char* password = "guest@@2022";  // Enter Password here

WebServer server(80);

constexpr size_t DEVICE_COUNT = 32;
constexpr size_t HR_FIELD_SIZE = 4;
constexpr size_t IR_FIELD_SIZE = 5;
uint16_t hrDatabase[DEVICE_COUNT][HR_FIELD_SIZE];
uint16_t irDatabase[DEVICE_COUNT][IR_FIELD_SIZE];

void writeEEPROM() {

  int address = 0;

  for (int i = 0; i < DEVICE_COUNT; i++) {
    for (int j = 0; j < HR_FIELD_SIZE; j++) {
      EEPROM.put(address, hrDatabase[i][j]);
      address += sizeof(uint16_t);
    }
  }

  EEPROM.commit();
}

void initDB() {
  int address = 0;

  for (int i = 0; i < DEVICE_COUNT; i++) {
    for (int j = 0; j < HR_FIELD_SIZE; j++) {
      Serial.println(EEPROM.read(address));
      hrDatabase[i][j] = EEPROM.read(address);
      address += sizeof(uint16_t);
    }
  }

  // for (size_t i = 0; i < DEVICE_COUNT; i++) {
  //   hrDatabase[i][1] = 2500;
  // }
  // for (size_t i = 0; i < DEVICE_COUNT; i++) {
  //   irDatabase[i][1] = 2500;
  // }
}

int counter = 0;

void setup() {
  EEPROM.begin(EEPROM_SIZE);

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

  server.onNotFound(handleNotFound);

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

  if (server.hasArg("power")) {
    hrDatabase[id][0] = server.arg("power").toInt();
  }

  if (server.hasArg("setTempIncrease")) {
    hrDatabase[id][1] += 50;
  }

  if (server.hasArg("setTempDecrease")) {
    hrDatabase[id][1] -= 50;
  }

  if (server.hasArg("mode")) {
    hrDatabase[id][2] = server.arg("mode").toInt();
  }

  if (server.hasArg("speed")) {
    hrDatabase[id][3] = server.arg("speed").toInt();
  }

  writeEEPROM();
}

void handleFetchAll() {
  String str = "";

  str += "{";
  str += "HR:[";
  for (size_t i = 0; i < DEVICE_COUNT; i++) {
    str += "[";
    for (size_t j = 0; j < HR_FIELD_SIZE; j++) {
      str += hrDatabase[i][j];
      str += ",";
    }
    str += "],";
  }
  str += "],";
  str += "IR:[";
  for (size_t i = 0; i < DEVICE_COUNT; i++) {
    str += "[";
    for (size_t j = 0; j < IR_FIELD_SIZE; j++) {
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

void handleNotFound() {
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