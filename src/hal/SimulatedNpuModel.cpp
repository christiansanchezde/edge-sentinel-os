#include "hal/SimulatedNpuModel.hpp"

#include "Logger.hpp"

namespace edge::hal {

bool SimulatedNpuModel::LoadModel(const std::string& model_path) {
    LOG_INFO("[NPU-SIM] Simulated Model Loaded.");
    return true;  // Always succeeds
}

float SimulatedNpuModel::RunInference(const SensorData& data) {
    LOG_DEBUG("[NPU-SIM] Running heuristic inference...");

    // Our simulation heuristic logic
    if (data.temperature > 40.0f && data.humidity < 30.0f) return 0.85f;
    if (data.temperature > 50.0f) return 0.95f;
    return 0.10f;
}

}  // namespace edge::hal