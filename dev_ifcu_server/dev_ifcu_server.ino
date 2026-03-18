#include <WiFi.h>
#include <WebServer.h>

String jsonString = "";

const int MAX_ROWS = 10;
const int MAX_COLUMNS = 9;

String database[MAX_ROWS][MAX_COLUMNS] = {
  { "ifcu-001", "", "", "", "", "", "", "", "" },
  { "ifcu-002", "", "", "", "", "", "", "", "" },
};

String* findRowByKey(String key) {
  for (int i = 0; i < MAX_COLUMNS; i++) {
    if (database[i][0] == key) {
      return database[i];
    }
  }
  return nullptr;
}

String convert2DArrayToJSON() {
  jsonString = "[";
  for (int i = 0; i < MAX_ROWS; i++) {
    jsonString += "[";
    for (int j = 0; j < MAX_COLUMNS; j++) {
      jsonString += '\"';
      jsonString += database[i][j];
      jsonString += '\"';
      if (j < MAX_COLUMNS - 1) {
        jsonString += ",";
      }
    }
    jsonString += "]";
    if (i < MAX_ROWS - 1) {
      jsonString += ",";
    }
  }

  jsonString += "]";
  return jsonString;
}

const char* ssid = "REC Guest - 16F";
const char* password = "guest@@2022";

// Create a WebServer object on port 80
WebServer server(80);

// Function to handle the /device/get/ endpoint
void handleDeviceGet() {
  // Check if parameters exist in the request
  if (server.hasArg("id")) {
    String id = server.arg("id");
    String onOff = server.arg("onOff");
    String mode = server.arg("mode");
    String speed = server.arg("speed");
    String setTemp = server.arg("setTemp");
    String roomTemp = server.arg("roomTemp");
    String isConn = server.arg("isConn");

    String* row = findRowByKey(id);
    if (row != nullptr) {
      row[2] = onOff;
      row[3] = mode;
      row[4] = speed;
      row[5] = setTemp;
      row[6] = roomTemp;
      row[7] = isConn;
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

void handleBrowserGet() {
  convert2DArrayToJSON();
  server.send(200, "application/json", jsonString);
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


  server.enableCORS();
  // Define the route and bind it to the handler function
  server.on("/device/get", HTTP_GET, handleDeviceGet);
  server.on("/browser/set", HTTP_GET, handleBrowserSet);
  server.on("/browser/get", HTTP_GET, handleBrowserGet);

  // Start the server
  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}