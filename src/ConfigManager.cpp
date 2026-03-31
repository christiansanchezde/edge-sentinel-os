#include "ConfigManager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

// This macro will be provided by CMake later. Fallback if missing.
#ifndef PROJECT_FW_VERSION
#define PROJECT_FW_VERSION "0.0.0-unknown"
#endif

namespace edge::core {

ConfigManager::ConfigManager() {
    // Set default fallbacks and bake in the compile-time FW version
    currentConfig.fwVersion = PROJECT_FW_VERSION;
    currentConfig.i2cBus = "/dev/i2c-0";
    currentConfig.i2cAddress = 0x77;
    currentConfig.logLevel = "INFO";
}

bool ConfigManager::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Config] WARNING: Could not open " << filepath << ". Using defaults.\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                
                // Route the values to the struct
                if (key == "I2C_BUS") {
                    currentConfig.i2cBus = value;
                } else if (key == "I2C_ADDRESS") {
                    // Convert hex string (e.g., "0x77" or "77") to integer
                    currentConfig.i2cAddress = std::stoi(value, nullptr, 16);
                } else if (key == "LOG_LEVEL") {
                    currentConfig.logLevel = value;
                }
            }
        }
    }
    
    std::cout << "[Config] Loaded configuration from " << filepath << "\n";
    return true;
}

} // namespace edge::core