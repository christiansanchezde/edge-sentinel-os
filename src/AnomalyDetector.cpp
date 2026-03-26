#include "AnomalyDetector.hpp"

namespace edge::app {

AnomalyDetector::AnomalyDetector(edge::hal::ISensor& s) : sensor(s) {}

bool AnomalyDetector::checkForFire() {
    edge::hal::SensorData currentData = sensor.readData();
    // Simple logic: If temp > 50.0C, return true (Fire detected!)
    return currentData.temperature > 50.0f;
}

} // namespace edge::app