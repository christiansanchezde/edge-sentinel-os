#pragma once
#include <string>

namespace edge::core {

// A simple struct to hold our active settings
struct AppConfig {
    std::string fwVersion;
    std::string i2cBus;
    int i2cAddress;
    std::string logLevel;
};

class ConfigManager {
private:
    AppConfig currentConfig;

    // Private constructor prevents direct instantiation
    ConfigManager(); 

public:
    // Delete copy/move constructors to enforce Singleton
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // The single point of access
    static ConfigManager& getInstance() {
        static ConfigManager instance; // Guaranteed to be destroyed and instantiated on first use
        return instance;
    }

    // Loads the file and populates the AppConfig struct
    bool load(const std::string& filepath);

    // Read-only access to the configuration
    const AppConfig& getConfig() const { return currentConfig; }
};

} // namespace edge::core