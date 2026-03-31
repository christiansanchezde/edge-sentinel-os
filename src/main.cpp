#include <thread>
#include <chrono>
#include "hal/BME280.hpp"
#include "AnomalyDetector.hpp"
#include "ConfigManager.hpp"
#include "Logger.hpp"

using namespace edge::hal;
using namespace edge::app;
using namespace edge::core;

int main() {
    // 1. Load Configuration
    auto& config_mgr = ConfigManager::GetInstance();
    config_mgr.Load("config.env");
    const AppConfig& config = config_mgr.GetConfig();

    // 2. Configure Logger based on the .env file
    Logger::GetInstance().SetLevelFromString(config.logLevel);

    LOG_INFO("======================================");
    LOG_INFO(" Edge Sentinel OS");
    LOG_INFO(" Firmware Version: " << config.fwVersion);
    LOG_INFO("======================================");

    // 3. Initialize Hardware
    BME280 my_sensor(config.i2cBus, config.i2cAddress); 
    
    if (!my_sensor.Init()) {
        LOG_FATAL("Sensor initialization failed on " << config.i2cBus << ". Exiting.");
        return -1;
    }

    // 4. Inject Dependency
    AnomalyDetector detector(my_sensor);

    // 5. Main Loop
    LOG_INFO("Entering monitoring loop...");
    
    for (int i = 0; i < 5; ++i) { 
        SensorData data = my_sensor.ReadData();
        
        LOG_DEBUG("Readings -> Temp: " << data.temperature 
                  << "C, Hum: " << data.humidity 
                  << "%, Pres: " << data.pressure << " hPa");

        if (detector.CheckForFire()) {
            LOG_WARN("🚨 ANOMALY DETECTED! TEMPERATURE EXCEEDS THRESHOLD 🚨");
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    LOG_INFO("Shutting down Edge Sentinel OS.");
    return 0;
}