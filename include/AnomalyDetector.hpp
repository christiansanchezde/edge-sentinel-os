#pragma once
#include "hal/ISensor.hpp"

namespace edge::app {

class AnomalyDetector {
private:
    edge::hal::ISensor& sensor; // Dependency Injection

public:
    // Explicit constructor to prevent accidental implicit conversions
    explicit AnomalyDetector(edge::hal::ISensor& s);
    
    bool checkForFire();
};

} // namespace edge::app