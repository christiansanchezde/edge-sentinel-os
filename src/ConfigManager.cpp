#include "ConfigManager.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#ifndef PROJECT_FW_VERSION
#define PROJECT_FW_VERSION "0.0.0-unknown"
#endif

namespace edge::core {

ConfigManager::ConfigManager() {
    current_config_.fwVersion = PROJECT_FW_VERSION;
    current_config_.i2cBus = "/dev/i2c-0";
    current_config_.i2cAddress = 0x77;
    current_config_.aiMode = "SIMULATED";
}

bool ConfigManager::Load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Config] WARNING: Could not open " << filepath << ". Using defaults.\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                if (key == "I2C_BUS") {
                    current_config_.i2cBus = value;
                } else if (key == "I2C_ADDRESS") {
                    current_config_.i2cAddress = std::stoi(value, nullptr, 16);
                } else if (key == "AI_MODE") {
                    current_config_.aiMode = value;
                }
                // Removed the LOG_LEVEL string parser here
            }
        }
    }

    std::cout << "[Config] Loaded configuration from " << filepath << "\n";
    return true;
}

}  // namespace edge::core