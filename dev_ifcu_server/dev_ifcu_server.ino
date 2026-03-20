#include <WiFi.h>
#include <WebServer.h>

String jsonString = "";

const int MAX_ROWS = 10;

class Record {
public:
  String id;
  String name;
  String cmd;
  String onOff;
  String mode;
  String speed;
  String setTemp;
  String roomTemp;
  String isSynced;
  uint32_t lastCommAt;
  bool isConnected;

  Record(String id = "",
         String name = "",
         String cmd = "",
         String onOff = "",
         String mode = "",
         String speed = "",
         String setTemp = "",
         String roomTemp = "",
         String isSynced = "",
         uint32_t lastCommAt = millis(),
         bool isConnected = 0)
    : id(id),
      name(name),
      cmd(cmd),
      onOff(onOff),
      mode(mode),
      speed(speed),
      setTemp(setTemp),
      roomTemp(roomTemp),
      isSynced(isSynced),
      lastCommAt(lastCommAt),
      isConnected(isConnected) {
  }
};

class RecordDB {
private:
  Record recordDatabase[MAX_ROWS] = {
    Record("ifcu-001", "IFCU-001", "", "", "", "", "", "", "", millis(), false),
    Record("ifcu-002", "IFCU-002", "", "", "", "", "", "", "", millis(), false),
  };
public:
  RecordDB() {}

  Record* findDevice(String id) {
    for (int i = 0; i < MAX_ROWS; i++) {
      if ((this->recordDatabase[i]).id == id) {
        return &this->recordDatabase[i];
      }
    }
    return nullptr;
  }

  String toJSON() {
    jsonString = "[";
    for (size_t i = 0; i < MAX_ROWS; i++) {
      jsonString += "[";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).id;
      jsonString += '\"';
      jsonString += ",";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).name;
      jsonString += '\"';
      jsonString += ",";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).cmd;
      jsonString += '\"';
      jsonString += ",";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).onOff;
      jsonString += '\"';
      jsonString += ",";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).mode;
      jsonString += '\"';
      jsonString += ",";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).speed;
      jsonString += '\"';
      jsonString += ",";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).setTemp;
      jsonString += '\"';
      jsonString += ",";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).roomTemp;
      jsonString += '\"';
      jsonString += ",";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).isSynced;
      jsonString += '\"';
      jsonString += ",";

      jsonString += '\"';
      jsonString += (this->recordDatabase[i]).isConnected;
      jsonString += '\"';

      jsonString += "]";

      if (i < MAX_ROWS - 1) {
        jsonString += ",";
      }
    }

    jsonString += "]";

    Serial.println(jsonString);
    return jsonString;
  }
};

RecordDB recordDB;

const char* ssid = "REC Guest - 16F";
const char* password = "guest@@2022";

WebServer server(80);

void handleDeviceGet() {
  if (server.hasArg("id")) {
    String id = server.arg("id");
    String onOff = server.arg("onOff");
    String mode = server.arg("mode");
    String speed = server.arg("speed");
    String setTemp = server.arg("setTemp");
    String roomTemp = server.arg("roomTemp");
    String isSynced = server.arg("isSynced");

    Record* record = recordDB.findDevice(id);
    if (record != nullptr) {
      record->onOff = onOff;
      record->mode = mode;
      record->speed = speed;
      record->setTemp = setTemp;
      record->roomTemp = roomTemp;
      record->isSynced = isSynced;

      if (millis() - record->lastCommAt <= 10000) {
        record->lastCommAt = millis();
        record->isConnected = true;
      } else {
        record->isConnected = false;
      }

      server.send(200, "text/plain", record->cmd);
      record->cmd = "";
    } else {
      Serial.println("ID not found, cannot update!");
    }
  } else {
    server.send(400, "text/plain", "Missing parameters.");
  }
}

void handleBrowserSet() {
  if (server.hasArg("id") && server.hasArg("cmd")) {
    String id = server.arg("id");
    String cmd = server.arg("cmd");

    Record* record = recordDB.findDevice(id);
    if (record != nullptr) {
      record->cmd += cmd;
      server.send(200, "text/plain", record->id);
    } else {
      Serial.println("ID not found, cannot update!");
    }
  } else {
    server.send(400, "text/plain", "Missing parameters.");
  }
}

void handleBrowserGet() {
  server.send(200, "application/json", recordDB.toJSON());
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

  server.on("/device/get", HTTP_GET, handleDeviceGet);
  server.on("/browser/set", HTTP_GET, handleBrowserSet);
  server.on("/browser/get", HTTP_GET, handleBrowserGet);
  server.on("/", HTTP_GET, handleBrowserGet);

  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
}