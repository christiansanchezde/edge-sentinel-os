#pragma once
#include <string>

#include "INpuModel.hpp"

namespace edge::hal {

class RknnModel : public INpuModel {
   private:
    bool is_loaded_;
    // In a full implementation, store Rockchip context variables here:
    // rknn_context ctx_;

   public:
    RknnModel();
    ~RknnModel() override;

    bool LoadModel(const std::string& model_path) override;
    float RunInference(const SensorData& data) override;
};

}  // namespace edge::hal