#include <WiFi.h>
#include <WebServer.h>

const int MAX_ROWS = 9;
const int MAX_COLUMNS = 10;

String database[MAX_ROWS][MAX_COLUMNS] = {
  { "ifcu-001", "ABC", "", "", "", "", "", "" },
  { "ifcu-002", "ABC", "", "", "", "", "", "" },
};

String* findRowByKey(String key) {
  for (int i = 0; i < MAX_COLUMNS; i++) {
    if (database[i][0] == key) {
      return database[i];
    }
  }
  return nullptr;
}

void updateRowByKey(String id, String onOff, String mode, String speed, String setTemp, String isConn) {
  String* row = findRowByKey(id);
  if (row != nullptr) {
    row[2] = onOff;
    row[3] = mode;
    row[4] = speed;
    row[5] = setTemp;
    row[6] = isConn;
    Serial.println("Row updated successfully!");
  } else {
    Serial.println("Key not found, cannot update!");
  }

  Serial.println("Data after modification:");
  for (int i = 0; i < MAX_ROWS; i++) {
    for (int j = 0; j < MAX_COLUMNS; j++) {
      Serial.print(database[i][j] + " ");
    }
    Serial.println();
  }
}

// Replace with your network credentials
const char* ssid = "REC Guest - 16F";
const char* password = "guest@@2022";

// Create a WebServer object on port 80
WebServer server(80);

// Function to handle the /device/get/ endpoint
void handleDeviceGet() {
  // Check if parameters exist in the request
  if (server.hasArg("id") && server.hasArg("onOff") && server.hasArg("mode") && server.hasArg("speed") && server.hasArg("setTemp") && server.hasArg("isConn")) {
    String id = server.arg("id");
    String onOff = server.arg("onOff");
    String mode = server.arg("mode");
    String speed = server.arg("speed");
    String setTemp = server.arg("setTemp");
    String isConn = server.arg("isConn");

    // Create a response message
    String response = "Received parameters:\n";
    response += "id: " + id + "\n";
    response += "mode: " + mode + "\n";
    response += "speed: " + speed + "\n";
    response += "setTemp: " + setTemp + "\n";

    // modify database
    updateRowByKey(id, onOff, mode, speed, setTemp, isConn);

    String* row = findRowByKey(id);
    if (row != nullptr) {
      server.send(200, "text/plain", row[1]);
      row[1] = "";
    } else {
      Serial.println("Key not found, cannot update!");
    }
  } else {
    // If parameters are missing, send an error response
    server.send(400, "text/plain", "Missing parameters.");
  }
}

void handleBrowserSet() {
  if (server.hasArg("id") && server.hasArg("cmd")) {
    String id = server.arg("id");
    String cmd = server.arg("cmd");

    String* row = findRowByKey(id);
    if (row != nullptr) {
      row[1] = cmd;
      server.send(200, "text/plain", "[" + row[2] + "]");
    } else {
      Serial.println("Key not found, cannot update!");
    }
  } else {
    // If parameters are missing, send an error response
    server.send(400, "text/plain", "Missing parameters.");
  }
}

void setup() {
  // Start the Serial connection
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected. IP Address: " + WiFi.localIP().toString());

  // Define the route and bind it to the handler function
  server.on("/device/get", HTTP_GET, handleDeviceGet);
  server.on("/browser/set", HTTP_GET, handleBrowserSet);

  // Start the server
  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}