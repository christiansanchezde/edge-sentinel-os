#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "hal/ISensor.hpp"
#include "hal/INpuModel.hpp"
#include "AnomalyDetector.hpp"

using namespace edge::hal;
using namespace edge::app;
using ::testing::Return;
using ::testing::_;

// ==========================================
// 1. THE MOCKS
// ==========================================
class MockSensor : public ISensor {
public:
    MOCK_METHOD(bool, Init, (), (override));
    MOCK_METHOD(SensorData, ReadData, (), (override));
};

class MockNpuModel : public INpuModel {
public:
    MOCK_METHOD(bool, LoadModel, (const std::string& model_path), (override));
    MOCK_METHOD(float, RunInference, (const SensorData& data), (override));
};

// ==========================================
// 2. THE TEST FIXTURE (Setup & Helpers)
// ==========================================
class AnomalyDetectorTest : public ::testing::Test {
protected:
    MockSensor mock_sensor_;
    MockNpuModel mock_npu_;
    std::unique_ptr<AnomalyDetector> detector_;

    // SetUp runs automatically before EVERY test
    void SetUp() override {
        detector_ = std::make_unique<AnomalyDetector>(mock_sensor_, mock_npu_);
    }

    // --- Helper Methods to abstract the Mocking Logic ---
    void SetMockSensorData(float temp, float humidity, float pressure) {
        EXPECT_CALL(mock_sensor_, ReadData())
            .WillOnce(Return(SensorData{temp, humidity, pressure}));
    }

    void SetMockNpuScore(float score) {
        EXPECT_CALL(mock_npu_, RunInference(_))
            .WillOnce(Return(score));
    }
};

// ==========================================
// 3. THE EXPLICIT TEST LOGIC
// ==========================================

TEST_F(AnomalyDetectorTest, TriggersAlarmOnHighAnomalyScore) {
    // Arrange (using our clean helpers)
    SetMockSensorData(60.0f, 20.0f, 1013.0f);
    SetMockNpuScore(0.95f);

    // Act
    AnomalyReport report = detector_->AnalyzeData();

    // Assert
    EXPECT_TRUE(report.anomaly_detected);
    EXPECT_FLOAT_EQ(report.ai_score, 0.95f);
}

TEST_F(AnomalyDetectorTest, NoAlarmOnNormalScore) {
    // Arrange
    SetMockSensorData(22.0f, 40.0f, 1013.0f);
    SetMockNpuScore(0.10f);

    // Act
    AnomalyReport report = detector_->AnalyzeData();

    // Assert
    EXPECT_FALSE(report.anomaly_detected);
    EXPECT_FLOAT_EQ(report.ai_score, 0.30f);
}