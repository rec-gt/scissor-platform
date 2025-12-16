#ifndef MDNS_Service_H
#define MDNS_Service_H
#include "Globals.h"

class MDNSService {
private:
  uint16_t prevMillis = millis();

  void start() {
    if (mDNSStatus == MDNS_EMPTY) {
      if (MDNS.begin(mDNSHostname)) {
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
      IPAddress serverIp = MDNS.queryHost(gatewayHostname);
      int n = MDNS.queryService("http", "tcp");

      if (serverIp.toString() == "0.0.0.0") {
        uint16_t currMillis = millis();
        if (currMillis - this->prevMillis >= 1000) {
          this->prevMillis = currMillis;
          Serial.println("Still looking for server IP...");
          serverIp = MDNS.queryHost(gatewayHostname);
          mDNSStatus = MDNS_PROBE;
        }
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

  bool isConnected() {
    return mDNSStatus == MDNS_PROBE_FINISH;
  }

  ~MDNSService() {}
};

extern MDNSService mDNSService;

#endif