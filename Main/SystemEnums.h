#ifndef SystemEnums_h
#define SystemEnums_h

enum SystemStatus {
  INIT,
  RUNNING,
  STOPPED,
  ALLOW_10S,
  FAILURE
};

const char* STATUS_STR[] = { "INIT", "RUNNING", "STOPPED", "ALLOW_10S", "FAILURE" };

enum TrafficStatus {
  RED,
  YELLOW,
  GREEN
};

#endif
