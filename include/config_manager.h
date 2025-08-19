#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <nlohmann/json.hpp>
#include <string>
#include <mutex>

struct Target {
    std::string url;
    int interval;
};

class ConfigManager {
public:
    ConfigManager(const std::string& config_path = "config.json");

    nlohmann::json getConfig();
    void updateConfig(const nlohmann::json& new_config);
    void addTarget(const Target& new_target);
    void deleteTarget(const std::string& url);

private:
    std::string config_path_;
    nlohmann::json config_data_;
    std::mutex config_mutex_;

    void loadConfig();
    void saveConfig();
};

#endif // CONFIG_MANAGER_H