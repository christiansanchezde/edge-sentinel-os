#include <thread>
#include <chrono>
#include "hal/BME280.hpp"
#include "hal/RknnModel.hpp" // Bring in the NPU!
#include "AnomalyDetector.hpp"
#include "ConfigManager.hpp"
#include "Logger.hpp"

using namespace edge::hal;
using namespace edge::app;
using namespace edge::core;

int main() {
    // 1. Load Configuration & Setup Logger
    auto& config_mgr = ConfigManager::GetInstance();
    config_mgr.Load("config.env");
    const AppConfig& config = config_mgr.GetConfig();
    Logger::GetInstance().SetLevelFromString(config.logLevel);

    LOG_INFO("======================================");
    LOG_INFO(" Edge Sentinel OS - AI Powered");
    LOG_INFO(" Firmware Version: " << config.fwVersion);
    LOG_INFO("======================================");

    // 2. Initialize Hardware Sensor
    BME280 my_sensor(config.i2cBus, config.i2cAddress); 
    if (!my_sensor.Init()) {
        LOG_FATAL("Sensor initialization failed. Exiting.");
        return -1;
    }

    // 3. Initialize Rockchip NPU Model
    RknnModel my_npu;
    if (!my_npu.LoadModel("models/fire_detection.rknn")) {
        LOG_FATAL("Failed to load Edge AI model into NPU. Exiting.");
        return -1;
    }

    // 4. Inject Dependencies into the Business Logic
    AnomalyDetector detector(my_sensor, my_npu);

    // 5. Main Loop
    LOG_INFO("Entering AI monitoring loop...");
    
    for (int i = 0; i < 5; ++i) { 
        // We do a raw print of the data here just so we can see it on screen
        SensorData data = my_sensor.ReadData();
        LOG_INFO("Readings -> Temp: " << data.temperature 
                 << "C, Hum: " << data.humidity 
                 << "%, Pres: " << data.pressure << " hPa");

        // The detector handles the AI inference under the hood
        if (detector.CheckForFire()) {
            LOG_WARN("🚨 NPU DETECTED SEVERE ANOMALY 🚨");
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    LOG_INFO("Shutting down Edge Sentinel OS.");
    return 0;
}