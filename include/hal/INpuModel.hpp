#pragma once
#include "ISensor.hpp"
#include <string>

namespace edge::hal {

class INpuModel {
public:
    virtual ~INpuModel() = default;

    // Load the .rknn model file into the hardware NPU
    virtual bool LoadModel(const std::string& model_path) = 0; //pure virtual

    // Feed sensor data to the NPU and get an anomaly score (0.0 to 1.0)
    // Values closer to 1.0 indicate a severe anomaly (e.g., Fire)
    virtual float RunInference(const SensorData& data) = 0;
};

} // namespace edge::hal