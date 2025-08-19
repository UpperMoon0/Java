#include "config_manager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

ConfigManager::ConfigManager(const std::string& config_path) : config_path_(config_path) {
    loadConfig();
}

nlohmann::json ConfigManager::getConfig() {
    std::lock_guard<std::mutex> lock(config_mutex_);
    return config_data_;
}

void ConfigManager::updateConfig(const nlohmann::json& new_config) {
    {
        std::lock_guard<std::mutex> lock(config_mutex_);
        config_data_ = new_config;
    }
    saveConfig();
}

void ConfigManager::addTarget(const Target& new_target) {
    {
        std::lock_guard<std::mutex> lock(config_mutex_);
        nlohmann::json target_json;
        target_json["url"] = new_target.url;
        target_json["interval"] = new_target.interval;
        config_data_["targets"].push_back(target_json);
    }
    saveConfig();
}

void ConfigManager::deleteTarget(const std::string& url) {
    {
        std::lock_guard<std::mutex> lock(config_mutex_);
        auto& targets = config_data_["targets"];
        targets.erase(std::remove_if(targets.begin(), targets.end(),
                                     [&](const nlohmann::json& target) {
                                         return target["url"] == url;
                                     }),
                      targets.end());
    }
    saveConfig();
}

void ConfigManager::loadConfig() {
    std::ifstream f(config_path_);
    if (!f.is_open()) {
        std::cout << "Could not open " << config_path_ << ". Creating a new one." << std::endl;
        config_data_ = {
            {"targets", nlohmann::json::array()}
        };
        saveConfig();
    } else {
        try {
            config_data_ = nlohmann::json::parse(f);
        } catch (nlohmann::json::parse_error& e) {
            std::cerr << "Failed to parse config file: " << e.what() << std::endl;
            config_data_ = {
                {"targets", nlohmann::json::array()}
            };
        }
    }
}

void ConfigManager::saveConfig() {
    std::ofstream o(config_path_);
    o << std::setw(4) << config_data_ << std::endl;
}