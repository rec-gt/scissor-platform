#ifndef DATABASE_H
#define DATABASE_H

class Database {
private:
  uint16_t IR_DATABASE[16] = {};
  uint16_t HR_DATABASE[16] = {};

public:
  Database() {}

  void setIRDB(size_t i, uint16_t value) {
    this->IR_DATABASE[i] = value;
  }

  ~Database() {}
};

extern Database database;

#endif