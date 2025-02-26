#ifndef SystemEnums_h
#define SystemEnums_h

enum SystemStatus {
  SYS_INIT,
  SYS_RUNNING,
  SYS_STOPPED,
  SYS_ALLOW_10S,
  SYS_FAILURE
};

const char* STATUS_STR[] = { "SYS_INIT", "SYS_RUNNING", "SYS_STOPPED", "SYS_ALLOW_10S", "SYS_FAILURE" };

enum TrafficStatus {
  TRAFFIC_RED,
  TRAFFIC_YELLOW,
  TRAFFIC_GREEN
};

#endif
