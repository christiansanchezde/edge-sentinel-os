#pragma once
#include "hal/ISensor.hpp"

namespace edge::app {

class AnomalyDetector {
private:
    edge::hal::ISensor& sensor_; // Dependency Injection

public:
    // Explicit constructor to prevent accidental implicit conversions
    explicit AnomalyDetector(edge::hal::ISensor& s);
    
    bool CheckForFire();
};

} // namespace edge::app