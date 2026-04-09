#include <memory> // For std::unique_ptr
#include <thread>
#include <chrono>
#include "hal/BME280.hpp"
#include "hal/RknnModel.hpp"
#include "hal/SimulatedNpuModel.hpp" // Bring in the Simulator
#include "hal/SqliteStorage.hpp"
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

    LOG_INFO("======================================");
    LOG_INFO(" Edge Sentinel OS (" << config.aiMode << " AI)");
    LOG_INFO(" Firmware Version: " << config.fwVersion);
    LOG_INFO("======================================");

    // 1. Initialize Hardware Sensor
    BME280 my_sensor(config.i2cBus, config.i2cAddress); 
    if (!my_sensor.Init()) {
        LOG_CRITICAL("Sensor initialization failed. Exiting.");
        return -1;
    }

    // 2. Initialize the AI Model (Factory Pattern via Config)
    std::unique_ptr<INpuModel> ai_model;
    
    if (config.aiMode == "HARDWARE") {
        LOG_INFO("Initializing Rockchip Hardware NPU...");
        ai_model = std::make_unique<RknnModel>();
    } else {
        LOG_INFO("Initializing Software Simulated NPU...");
        ai_model = std::make_unique<SimulatedNpuModel>();
    }

    if (!ai_model->LoadModel("models/fire_detection.rknn")) {
        LOG_CRITICAL("Failed to load AI model. Exiting.");
        return -1;
    }

    // 3. Initialize SQLite
    SqliteStorage local_db("edge_data.sqlite");
    if (!local_db.Init()) {
        LOG_CRITICAL("Failed to initialize database. Exiting.");
        return -1;
    }

    // 4. Inject Dependencies (Note the * to dereference the unique_ptr)
    AnomalyDetector detector(my_sensor, *ai_model);

    // 5. Main Loop
    LOG_INFO("Entering monitoring loop...");
    for (int i = 0; i < 5; ++i) { 
        SensorData data = my_sensor.ReadData();
        LOG_INFO("Temp: " << data.temperature << "C, Hum: " << data.humidity << "%, Pres: " << data.pressure << " hPa");

        AnomalyReport report = detector.AnalyzeData();

        if (report.anomaly_detected) {
            LOG_WARN("🚨 ANOMALY DETECTED (Score: " << report.ai_score << ") 🚨");
        }

        local_db.LogReading(data, report.ai_score);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    LOG_INFO("Shutting down Edge Sentinel OS.");
    return 0;
}