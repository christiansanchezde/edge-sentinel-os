#include "hal/AipuModel.hpp"

#include "Logger.hpp"

namespace edge::hal {


// 3TOPS NPU based on the Zhouyi AIPU (Arm) architecture.

AipuModel::AipuModel() : is_loaded_(false) {
}

AipuModel::~AipuModel() {
    if (is_loaded_) {
        // aipu_destroy(ctx_);
        LOG_INFO("[NPU] Rockchip NPU context destroyed safely.");
    }
}

bool AipuModel::LoadModel(const std::string& model_path) {
    LOG_INFO("[NPU] Attempting to load model from: " << model_path);

    // --- MOCK AIPU C-API IMPLEMENTATION ---
    // Here is where you would call:
    // int ret = aipu_init(&ctx_, model_data, model_size, 0, NULL);

    // For this portfolio PoC, we simulate a successful NPU load
    is_loaded_ = true;
    LOG_INFO("[NPU] Model loaded into hardware accelerator successfully.");
    return true;
}

float AipuModel::RunInference(const SensorData& data) {
    if (!is_loaded_) {
        LOG_ERROR("[NPU] Cannot run inference. Model not loaded!");
        return 0.0f;
    }

    LOG_DEBUG("[NPU] Feeding tensor data to NPU...");

    // TODO: Implement aipu_inputs_set() and aipu_run() API here

    return 0.0f;
}

}  // namespace edge::hal