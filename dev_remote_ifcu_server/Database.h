#ifndef DATABASE_H
#define DATABASE_H

class Database {
public:
  Database() {}

  void setIRDB(size_t i, uint16_t value) {
    IR_DATABASE[i] = value;
  }

  void getIRDB(size_t i, uint16_t value) {
    HR_DATABASE[i] = value;
  }

  ~Database() {}
};

extern Database database;

#endif