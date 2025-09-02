#include <WebServer.h>

#ifndef BackendServer_h
#define BackendServer_h

WebServer server(80);

class BackendServer {
public:
  BackendServer(){};

  void init() {
    server.on("/get", HTTP_GET, []() {
      
    });
    server.on("/set", HTTP_GET, []() {});
    server.onNotFound(handleNotFound);
  };

  ~BackendServer(){};
};

#endif