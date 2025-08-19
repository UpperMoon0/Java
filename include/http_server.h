#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <httplib.h>
#include "config_manager.h"
#include "keep_alive.h"

namespace keep_alive {
    class KeepAlive;
}

class HttpServer {
public:
    HttpServer(ConfigManager& configManager, keep_alive::KeepAlive& keepAlive);
    void start();
    void stop();

private:
    httplib::Server svr;
    ConfigManager& configManager;
    keep_alive::KeepAlive& keepAlive;
};

#endif // HTTP_SERVER_H