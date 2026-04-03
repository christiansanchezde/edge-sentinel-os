#include "AnomalyDetector.hpp"
#include "Logger.hpp"

namespace edge::app {

AnomalyDetector::AnomalyDetector(edge::hal::ISensor& sensor, edge::hal::INpuModel& npu) 
    : sensor_(sensor), npu_model_(npu) {}

bool AnomalyDetector::CheckForFire() {
    // 1. Get the real-time data
    edge::hal::SensorData current_data = sensor_.ReadData();

    // 2. Feed it to the NPU
    float anomaly_score = npu_model_.RunInference(current_data);

    LOG_DEBUG("AI Anomaly Score: " << anomaly_score);

    // 3. Evaluate the AI's prediction (e.g., > 80% certainty is a fire)
    return anomaly_score > 0.80f;
}

} // namespace edge::app