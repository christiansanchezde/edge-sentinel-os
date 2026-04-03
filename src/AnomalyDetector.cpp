#include "AnomalyDetector.hpp"
#include "Logger.hpp"

namespace edge::app {

AnomalyDetector::AnomalyDetector(edge::hal::ISensor& sensor, edge::hal::INpuModel& npu) 
    : sensor_(sensor), npu_model_(npu) {}

AnomalyReport AnomalyDetector::AnalyzeData() {
    // 1. Read directly from the injected sensor
    edge::hal::SensorData current_data = sensor_.ReadData();

    // 2. Feed data to the NPU
    float anomaly_score = npu_model_.RunInference(current_data);

    LOG_DEBUG("AI Anomaly Score: " << anomaly_score);

    // 3. Evaluate and return
    AnomalyReport report;
    report.ai_score = anomaly_score;
    report.anomaly_detected = (anomaly_score > 0.80f);

    return report;
}

} // namespace edge::app