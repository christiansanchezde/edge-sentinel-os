#pragma once
#include "hal/ISensor.hpp"
#include "hal/INpuModel.hpp"

namespace edge::app {

class AnomalyDetector {
private:
    edge::hal::ISensor& sensor_;     // The hardware sensor
    edge::hal::INpuModel& npu_model_; // The Edge AI model

public:
    // Now requires both the sensor and the AI model to be injected
    AnomalyDetector(edge::hal::ISensor& sensor, edge::hal::INpuModel& npu);
    
    bool CheckForFire();
};

} // namespace edge::app