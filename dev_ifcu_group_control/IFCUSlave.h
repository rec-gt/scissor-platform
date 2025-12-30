#include "Globals.h"
#include "NBIoT.h"

#ifndef IFCUSlave_H
#define IFCUSlave_H

class IFCUSlave {
private:
  byte slaveId;

public:
  IFCUSlave(byte slaveId)
    : slaveId(slaveId){};

  void write() {
    mbClientSlave.beginTransmission(this->slaveId, HOLDING_REGISTERS, 40000, 12);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.write(0);
    mbClientSlave.endTransmission();
  }
};

#endif
