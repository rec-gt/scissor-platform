#include <WiFi.h>
#include <WebServer.h>

/*Put your SSID & Password*/
const char* ssid = "REC Guest";        // Enter SSID here
const char* password = "guest@@2022";  // Enter Password here

WebServer server(80);

uint16_t hrDatabase[32][8];
uint16_t irDatabase[32][8];

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
  server.on("/set_target", HTTP_POST, handleSetTarget);

  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
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
  int currTemp;
  int setTemp;

  if (server.hasArg("id")) {
    id = server.arg("id").toInt();
  }

  if (server.hasArg("t2")) {
    String t2 = server.arg("t2");
    Serial.println("Received t2: " + t2);
  }

  if (server.hasArg("t3")) {
    String t3 = server.arg("t3");
    Serial.println("Received t3: " + t3);
  }
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