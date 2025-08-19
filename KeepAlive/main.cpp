#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <httplib.h>

using json = nlohmann::json;

struct Target {
    std::string url;
    int interval;
};

void keep_alive(const Target& target) {
    while (true) {
        auto response = cpr::Get(cpr::Url{target.url});
        if (response.status_code == 200) {
            std::cout << "Successfully pinged " << target.url << std::endl;
        } else {
            std::cerr << "Failed to ping " << target.url << " - Status code: " << response.status_code << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(target.interval));
    }
}

void start_http_server() {
    httplib::Server svr;
    svr.Get("/health", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("OK", "text/plain");
    });
    svr.listen("0.0.0.0", 8080);
}

int main() {
    std::ifstream f("config.json");
    if (!f.is_open()) {
        std::cerr << "Could not open config.json" << std::endl;
        return 1;
    }

    json data = json::parse(f);
    std::vector<Target> targets;
    for (const auto& item : data["targets"]) {
        targets.push_back({item["url"], item["interval"]});
    }

    std::vector<std::thread> threads;
    for (const auto& target : targets) {
        threads.emplace_back(keep_alive, target);
    }

    std::thread http_thread(start_http_server);

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    if (http_thread.joinable()) {
        http_thread.join();
    }

    return 0;
}