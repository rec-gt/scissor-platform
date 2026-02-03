#ifndef Database_H
#define Database_H
#include <EEPROM.h>

class Database {
private:

public:
  Database(void) {}

  void init() {
    EEPROM.put();
  }

  ~Database() {}
};

extern Database database;
#endif