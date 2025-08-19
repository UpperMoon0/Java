#ifndef KEEP_ALIVE_H
#define KEEP_ALIVE_H

#include "config_manager.h"
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <string>

namespace keep_alive {

class KeepAlive {
public:
    KeepAlive(ConfigManager& config_manager);
    ~KeepAlive();

    void start();
    void stop();
    void reload_config();

private:
    void worker_thread(const std::string& url, int interval);
    void send_keep_alive(const std::string& url);

    ConfigManager& config_manager_;
    std::vector<std::thread> worker_threads_;
    std::atomic<bool> running_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

} // namespace keep_alive

#endif // KEEP_ALIVE_H