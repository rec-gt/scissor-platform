class Database {
private:
  // WRITE ONLY
  uint16_t pendingDatabase[16] = {};
  uint16_t currentDatabase[16] = {};

public:
  Database() {}
  
  void set(size_t i) {}
  
  ~Database() {}
};
