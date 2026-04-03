#include <thread>
#include <chrono>
#include "hal/BME280.hpp"
#include "hal/RknnModel.hpp"
#include "hal/SqliteStorage.hpp" // Bring in the Database!
#include "AnomalyDetector.hpp"
#include "ConfigManager.hpp"
#include "Logger.hpp"

using namespace edge::hal;
using namespace edge::app;
using namespace edge::core;

int main() {
    auto& config_mgr = ConfigManager::GetInstance();
    config_mgr.Load("config.env");
    const AppConfig& config = config_mgr.GetConfig();
    Logger::GetInstance().SetLevelFromString(config.logLevel);

    LOG_INFO("======================================");
    LOG_INFO(" Edge Sentinel OS - AI Powered");
    LOG_INFO(" Firmware Version: " << config.fwVersion);
    LOG_INFO("======================================");

    // 1. Initialize Hardware & AI
    BME280 my_sensor(config.i2cBus, config.i2cAddress); 
    if (!my_sensor.Init()) {
        LOG_FATAL("Sensor initialization failed. Exiting.");
        return -1;
    }

    RknnModel my_npu;
    if (!my_npu.LoadModel("models/fire_detection.rknn")) {
        LOG_FATAL("Failed to load Edge AI model. Exiting.");
        return -1;
    }

    // 2. Initialize the SQLite Database
    SqliteStorage local_db("edge_data.sqlite");
    if (!local_db.Init()) {
        LOG_FATAL("Failed to initialize local database. Exiting.");
        return -1;
    }

    // 3. Inject Dependencies
    AnomalyDetector detector(my_sensor, my_npu);

    // 4. Main Application Loop
    LOG_INFO("Entering AI monitoring loop...");
    
    for (int i = 0; i < 5; ++i) { 
        // Read sensor
        SensorData data = my_sensor.ReadData();
        LOG_INFO("Readings -> Temp: " << data.temperature 
                 << "C, Hum: " << data.humidity 
                 << "%, Pres: " << data.pressure << " hPa");

        // Analyze with NPU
        AnomalyReport report = detector.AnalyzeData();

        if (report.is_fire) {
            LOG_WARN("🚨 NPU DETECTED SEVERE ANOMALY (Score: " << report.ai_score << ") 🚨");
        }

        // Save to Database
        local_db.LogReading(data, report.ai_score);

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    LOG_INFO("Shutting down Edge Sentinel OS.");
    return 0;
}