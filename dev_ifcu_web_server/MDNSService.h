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
      IPAddress serverIp = MDNS.queryHost(gatewayHostname);  // Returns 0.0.0.0 if not found

      while (serverIp.toString() == "0.0.0.0") {
        Serial.println("Still looking for server IP...");
        delay(250);
        serverIp = MDNS.queryHost(gatewayHostname);
      }
      gatewayIPAddress = serverIp.toString();
      Serial.println(gatewayIPAddress);
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