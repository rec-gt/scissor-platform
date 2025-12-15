#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "iFCUModbus.h"
#include "ESPmDNS.h"

const char* ssid = "REC Guest";        // Enter SSID here
const char* password = "guest@@2022";  // Enter Password here

HTTPClient http;

constexpr size_t DEVICE_COUNT = 32;
constexpr size_t HR_FIELD_SIZE = 4;
constexpr size_t IR_FIELD_SIZE = 5;
uint16_t hrDatabase[DEVICE_COUNT][HR_FIELD_SIZE];
uint16_t irDatabase[DEVICE_COUNT][IR_FIELD_SIZE];

String device_id = "1";
String getReqGetHR = "http://ifcu-web.local:3000/broker/get-hr/" + device_id;
String postReqSetIR = "http://ifcu-web.local:3000/broker/set-ir";

constexpr size_t arrayLength = 4 + 1;
uint16_t jsArray[arrayLength];

iFCUModbus ifcuModbus;

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

  if (!MDNS.begin("esp32")) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("Error starting mDNS");
    return;
  }

  Serial.println("Searching for web servers...");
  MDNS.queryService("http", "tcp");  // Find all devices offering HTTP
  delay(2000);                       // Wait for responses

  Serial.println("Found services:");
  int n = MDNS.queryService("http", "tcp");

  for (int i = 0; i < n; i++) {
    IPAddress ip = MDNS.address(i);
    String ip_string = ip.toString();
    Serial.println(i);
    Serial.println(MDNS.hostname(i));
    Serial.println(MDNS.port(i));
    Serial.println(ip_string);
  }

  Serial.println("mDNS responder started");

  ifcuModbus.init();
}

void loop() {
  handleGetAndSetHR();
  delay(500);
  handleSendIR();
  delay(500);
}


void handleGetAndSetHR() {
  http.begin(getReqGetHR);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    if (payload) {

      int startIndex = 1;  // Skip the opening bracket '['
      int endIndex = payload.indexOf(',', startIndex);
      for (int i = 0; i < arrayLength; i++) {
        if (endIndex == -1) {
          endIndex = payload.indexOf(']', startIndex);
        }
        jsArray[i] = payload.substring(startIndex, endIndex).toInt();
        startIndex = endIndex + 1;  // Skip the comma and space
        endIndex = payload.indexOf(',', startIndex);
      }

      if (jsArray[0] == 1) {
        Serial.println("Parsed array:");
        for (int i = 0; i < arrayLength; i++) {
          Serial.print(jsArray[i]);
          Serial.print(" ");
        }

        node.setTransmitBuffer(0, jsArray[1]);
        node.setTransmitBuffer(1, 0);
        node.setTransmitBuffer(2, jsArray[3]);
        node.setTransmitBuffer(3, jsArray[4]);
        node.setTransmitBuffer(4, jsArray[2]);
        result = node.writeMultipleRegisters(40000, 5);
        if (result == node.ku8MBSuccess) {
          Serial.println("Success");
        } else {
          Serial.println("Fail to update data");
        }
      }
    }

  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void handleSendIR() {
  result = node.readInputRegisters(30000, IR_SIZE);
  if (result == node.ku8MBSuccess) {
    for (size_t i = 0; i < IR_SIZE; i++) {
      IR_DATABASE[i] = node.getResponseBuffer(i);
    }
  } else {
    Serial.println("Cannot Fetch Data");
  }

  String baseURL = "http://ifcu-web.local:3000/broker/set-hr-device";

  String idParam = "id=" + device_id;
  String powerParam = "power=" + String((IR_DATABASE[1] & 0b01000000) != 0);
  String roomTempParam = "roomTemp=" + String(IR_DATABASE[5]);
  String setTempParam = "setTemp=" + String(IR_DATABASE[6]);
  String modeParam = "mode=" + String(IR_DATABASE[3]);
  String speedParam = "speed=" + String(IR_DATABASE[4]);
  String getReqSetHR = baseURL + "?" + idParam + "&" + powerParam + "&" + roomTempParam + "&" + setTempParam + "&" + modeParam + "&" + speedParam;

  http.begin(getReqSetHR);
  Serial.println(getReqSetHR);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.GET();
  Serial.println(httpResponseCode);
  http.end();
}