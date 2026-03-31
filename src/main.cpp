#include <iostream>
#include <thread>
#include <chrono>
#include "hal/BME280.hpp"
#include "AnomalyDetector.hpp"
#include "ConfigManager.hpp"

using namespace edge::hal;
using namespace edge::app;
using namespace edge::core;

int main() {
    // 1. Load Configuration
    auto& configMgr = ConfigManager::getInstance();
    configMgr.load("config.env");
    const AppConfig& config = configMgr.getConfig();

    std::cout << "======================================\n";
    std::cout << " Edge Sentinel OS\n";
    std::cout << " Firmware Version: " << config.fwVersion << "\n";
    std::cout << "======================================\n";

    // 2. Initialize Hardware using dynamic configuration
    BME280 mySensor(config.i2cBus, config.i2cAddress); 
    
    if (!mySensor.init()) {
        std::cerr << "[FATAL] Sensor initialization failed on " << config.i2cBus << ". Exiting.\n";
        return -1;
    }

    // 3. Inject Dependency
    AnomalyDetector detector(mySensor);

    // 4. Main Application Loop
    std::cout << "\n[INFO] Entering monitoring loop...\n";
    
    for (int i = 0; i < 5; ++i) { 
        SensorData data = mySensor.readData();
        
        std::cout << "Readings -> Temp: " << data.temperature 
                  << "C, Humidity: " << data.humidity 
                  << "%, Pressure: " << data.pressure << " hPa\n";

        if (detector.checkForFire()) {
            std::cout << "  🚨 ANOMALY DETECTED! TEMPERATURE EXCEEDS THRESHOLD 🚨\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    std::cout << "\n[INFO] Shutting down Edge Sentinel OS.\n";
    return 0;
}