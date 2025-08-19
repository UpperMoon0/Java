#include "http_server.h"
#include "config_manager.h"
#include "keep_alive.h"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<keep_alive::KeepAlive> keep_alive_ptr;
std::unique_ptr<HttpServer> http_server_ptr;

void signal_handler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    if (keep_alive_ptr) {
        keep_alive_ptr->stop();
    }
    if (http_server_ptr) {
        http_server_ptr->stop();
    }
    exit(signum);
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    ConfigManager config_manager("config.json");

    keep_alive_ptr = std::make_unique<keep_alive::KeepAlive>(config_manager);
    http_server_ptr = std::make_unique<HttpServer>(config_manager, *keep_alive_ptr);

    // No need to call load_config here, it's called in the constructor of ConfigManager

    keep_alive_ptr->start();
    
    std::cout << "Starting HTTP server on port 8080" << std::endl;
    http_server_ptr->start();

    return 0;
}