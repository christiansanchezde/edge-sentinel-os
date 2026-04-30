#pragma once
#include <string>

#include "INpuModel.hpp"

namespace edge::hal {

class AipuModel : public INpuModel {
   private:
    bool is_loaded_;
    // In a full implementation, store Rockchip context variables here:
    // rknn_context ctx_;

   public:
    AipuModel();
    ~AipuModel() override;

    bool LoadModel(const std::string& model_path) override;
    float RunInference(const SensorData& data) override;
};

}  // namespace edge::hal