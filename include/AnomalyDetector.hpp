#pragma once
#include "hal/ISensor.hpp"
#include "hal/INpuModel.hpp"

namespace edge::app {

struct AnomalyReport {
    bool is_fire;
    float ai_score;
};

class AnomalyDetector {
private:
    edge::hal::ISensor& sensor_;
    edge::hal::INpuModel& npu_model_;

public:
    AnomalyDetector(edge::hal::ISensor& sensor, edge::hal::INpuModel& npu);
    
    AnomalyReport AnalyzeData(); 
};

} // namespace edge::app