#include "keep_alive.h"
#include "config_manager.h"
#include <iostream>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <nlohmann/json.hpp>

namespace keep_alive {

KeepAlive::KeepAlive(ConfigManager& config_manager)
    : config_manager_(config_manager), running_(false) {}

KeepAlive::~KeepAlive() {
    stop();
}

void KeepAlive::start() {
    if (running_) {
        return;
    }
    running_ = true;
    reload_config();
}

void KeepAlive::stop() {
    if (!running_) {
        return;
    }
    running_ = false;
    cv_.notify_all();
    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();
}

void KeepAlive::reload_config() {
    stop();
    running_ = true;
    auto config = config_manager_.getConfig();
    for (const auto& target : config["targets"]) {
        worker_threads_.emplace_back(&KeepAlive::worker_thread, this, target["url"], target["interval"]);
    }
}

void KeepAlive::worker_thread(const std::string& url, int interval) {
    while (running_) {
        send_keep_alive(url);
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait_for(lock, std::chrono::seconds(interval), [this] { return !running_; });
    }
}

void KeepAlive::send_keep_alive(const std::string& url) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return;
    }
#endif

    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string host;
    std::string path = "/";
    size_t host_start = url.find("://");
    if (host_start != std::string::npos) {
        host_start += 3;
    } else {
        host_start = 0;
    }
    size_t path_start = url.find('/', host_start);
    if (path_start != std::string::npos) {
        host = url.substr(host_start, path_start - host_start);
        path = url.substr(path_start);
    } else {
        host = url.substr(host_start);
    }

    addrinfo* result = nullptr;
    if (getaddrinfo(host.c_str(), "80", &hints, &result) != 0) {
        std::cerr << "getaddrinfo failed for " << host << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    int sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == -1) {
        std::cerr << "socket creation failed" << std::endl;
        freeaddrinfo(result);
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) == -1) {
        std::cerr << "connect failed to " << url << std::endl;
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        freeaddrinfo(result);
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    std::string request = "GET " + path + " HTTP/1.1\r\n"
                        "Host: " + host + "\r\n"
                        "Connection: close\r\n\r\n";

    if (send(sock, request.c_str(), (int)request.length(), 0) == -1) {
        std::cerr << "send failed" << std::endl;
    } else {
        std::cout << "Keep-alive sent to " << url << std::endl;
    }

#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    freeaddrinfo(result);
#ifdef _WIN32
    WSACleanup();
#endif
}

} // namespace keep_alive