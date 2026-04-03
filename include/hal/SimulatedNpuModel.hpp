#pragma once
#include "INpuModel.hpp"
#include <string>

namespace edge::hal {

class SimulatedNpuModel : public INpuModel {
public:
    bool LoadModel(const std::string& model_path) override;
    float RunInference(const SensorData& data) override;
};

} // namespace edge::hal