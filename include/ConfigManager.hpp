#pragma once
#include <string>

namespace edge::core {

// A simple struct to hold our active settings
struct AppConfig {
    std::string fwVersion;
    std::string i2cBus;
    int i2cAddress;
    std::string aiMode;
};

class ConfigManager {
   private:
    AppConfig current_config_;

    // Private constructor prevents direct instantiation
    ConfigManager();

   public:
    // Delete copy/move constructors to enforce Singleton
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // The single point of access
    static ConfigManager& GetInstance() {
        static ConfigManager instance;  // Guaranteed to be destroyed and instantiated on first use
        return instance;
    }

    // Loads the file and populates the AppConfig struct
    bool Load(const std::string& filepath);

    // Read-only access to the configuration
    const AppConfig& GetConfig() const {
        return current_config_;
    }
};

}  // namespace edge::core