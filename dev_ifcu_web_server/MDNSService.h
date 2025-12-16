#ifndef MDNS_Service_H
#define MDNS_Service_H
#include "Globals.h"

class MDNSService {
private:
  uint16_t prevMillis = millis();

  void start() {
    if (mDNSStatus == MDNS_EMPTY) {
      if (MDNS.begin(mDNSHostname)) {
        MDNS.addService("http", "tcp", 80);
        mDNSStatus = MDNS_STARTED;
        Serial.println("Started mDNS");
      } else {
        Serial.println("Error starting mDNS");
      }
    }
  }

  void probe() {
    if (mDNSStatus == MDNS_STARTED) {
      mDNSStatus = MDNS_PROBE;
    }

    if (mDNSStatus == MDNS_PROBE) {
      int n = MDNS.queryService("http", "tcp");
      IPAddress serverIp = MDNS.queryHost(gatewayHostname);  // Returns 0.0.0.0 if not found

      if (serverIp.toString() == "0.0.0.0") {
        uint16_t currMillis = millis();
        Serial.println("Still looking for server IP...");
        serverIp = MDNS.queryHost(gatewayHostname);
        mDNSStatus = MDNS_PROBE;
      } else {
        gatewayIPAddress = serverIp.toString();
        Serial.println(gatewayIPAddress);
        mDNSStatus = MDNS_PROBE_FINISH;
      }
    }
  }

public:
  MDNSService() {}

  void loop() {
    this->start();
    this->probe();
  }

  ~MDNSService() {}
};

extern MDNSService mDNSService;

#endif