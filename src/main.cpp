#include <chrono>
#include <memory>
#include <thread>

#include "AnomalyDetector.hpp"
#include "ConfigManager.hpp"
#include "Logger.hpp"
#include "hal/BME280.hpp"
#include "hal/RknnModel.hpp"
#include "hal/SimulatedNpuModel.hpp"
#include "hal/SqliteStorage.hpp"

using namespace edge::hal;
using namespace edge::app;
using namespace edge::core;

int main() {
    // 1. Load System Configuration
    auto& config_mgr = ConfigManager::GetInstance();
    config_mgr.Load("config.env");
    const AppConfig& config = config_mgr.GetConfig();

    // 2. Initialize Hardware Sensor
    BME280 my_sensor(config.i2cBus, config.i2cAddress);
    if (!my_sensor.Init()) {
        LOG_CRITICAL("Sensor initialization failed. Exiting.");
        return -1;
    }

    // 3. Initialize the AI Model
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

    // 4. Initialize SQLite
    SqliteStorage local_db("edge_data.sqlite");
    if (!local_db.Init()) {
        LOG_CRITICAL("Failed to initialize database. Exiting.");
        return -1;
    }

    // 5. Setup Async Logger & Janitor
    auto& logger = edge::core::Logger::GetInstance();
    logger.SetDatabase(&local_db);
    
    // ARCHITECT NOTE: Run Janitor BEFORE the loop to clear space.
    // This executes synchronously to ensure we have disk space before logging starts.
    logger.PruneOldData(30); 

    LOG_INFO("======================================");
    LOG_INFO(" Edge Sentinel OS (" << config.aiMode << " AI)");
    LOG_INFO(" Pipeline: ASYNC LOGGER ACTIVE");
    LOG_INFO("======================================");

    // 6. Inject Dependencies
    AnomalyDetector detector(my_sensor, *ai_model);

    // 7. Main Monitoring Loop
    LOG_INFO("Entering monitoring loop...");
    for (;;) {
        // A. Read Sensor (Hardware I/O)
        SensorData data = my_sensor.ReadData();
        
        // B. Log to Async Queue (Non-blocking)
        LOG_INFO("Temp: " << data.temperature << "C, Hum: " << data.humidity
                          << "%, Pres: " << data.pressure << " hPa");

        // C. AI Inference
        AnomalyReport report = detector.AnalyzeData();

        if (report.anomaly_detected) {
            LOG_WARN("🚨 ANOMALY DETECTED (Score: " << report.ai_score << ") 🚨");
        }

        // D. Data Persistence
        // TODO: In the next phase, we should move this to an async worker too!
        local_db.LogReading(data, report.ai_score);

        // E. Loop Timing
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}