#include "hal/RknnModel.hpp"
#include "Logger.hpp"

namespace edge::hal {

RknnModel::RknnModel() : is_loaded_(false) {}

RknnModel::~RknnModel() {
    if (is_loaded_) {
        // rknn_destroy(ctx_);
        LOG_INFO("[NPU] Rockchip NPU context destroyed safely.");
    }
}

bool RknnModel::LoadModel(const std::string& model_path) {
    LOG_INFO("[NPU] Attempting to load model from: " << model_path);
    
    // --- MOCK RKNN C-API IMPLEMENTATION ---
    // Here is where you would call:
    // int ret = rknn_init(&ctx_, model_data, model_size, 0, NULL);
    
    // For this portfolio PoC, we simulate a successful NPU load
    is_loaded_ = true;
    LOG_INFO("[NPU] Model loaded into hardware accelerator successfully.");
    return true;
}

float RknnModel::RunInference(const SensorData& data) {
    if (!is_loaded_) {
        LOG_ERROR("[NPU] Cannot run inference. Model not loaded!");
        return 0.0f;
    }

    LOG_DEBUG("[NPU] Feeding tensor data to NPU...");

    // --- MOCK INFERENCE MATH ---
    // Here, you would pack 'data' into an rknn_input struct, call rknn_inputs_set(), 
    // call rknn_run(), and read the result via rknn_outputs_get().

    // We will simulate a simple anomaly heuristic: 
    // If temp is rising fast and humidity is dropping, the NPU outputs a high score.
    float anomaly_score = 0.0f;
    
    if (data.temperature > 40.0f && data.humidity < 30.0f) {
        anomaly_score = 0.85f; // High probability of fire/anomaly
    } else if (data.temperature > 50.0f) {
        anomaly_score = 0.95f; // Almost certain anomaly
    } else {
        anomaly_score = 0.10f; // Normal baseline
    }

    return anomaly_score;
}

} // namespace edge::hal