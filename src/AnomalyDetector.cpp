#include "AnomalyDetector.hpp"

namespace edge::app {

AnomalyDetector::AnomalyDetector(edge::hal::ISensor& s) : sensor_(s) {}

bool AnomalyDetector::CheckForFire() {
    edge::hal::SensorData currentData = sensor_.ReadData();
    // Simple logic: If temp > 50.0C, return true (Fire detected!)
    return currentData.temperature > 50.0f;
}

} // namespace edge::app