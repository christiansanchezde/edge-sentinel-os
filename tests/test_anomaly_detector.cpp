#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <memory>

#include "AnomalyDetector.hpp"
#include "hal/INpuModel.hpp"
#include "hal/ISensor.hpp"

using namespace edge::hal;
using namespace edge::app;
using ::testing::_;  // allows to ignore parameters in EXPECT_CALL when we don't care about them
using ::testing::Return;

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
        EXPECT_CALL(mock_npu_, RunInference(_)).WillOnce(Return(score));
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
    EXPECT_FLOAT_EQ(report.ai_score, 0.10f);
}

TEST_F(AnomalyDetectorTest, ExactlyAtThresholdDoesNotTriggerAlarm) {
    SetMockSensorData(22.0f, 40.0f, 1013.0f);
    SetMockNpuScore(0.80f);

    AnomalyReport report = detector_->AnalyzeData();

    EXPECT_FALSE(report.anomaly_detected);
    EXPECT_FLOAT_EQ(report.ai_score, 0.80f);
}

TEST_F(AnomalyDetectorTest, SlightlyAboveThresholdTriggersAlarm) {
    const float score = std::nextafter(0.80f, 1.0f);
    SetMockSensorData(22.0f, 40.0f, 1013.0f);
    SetMockNpuScore(score);

    AnomalyReport report = detector_->AnalyzeData();

    EXPECT_TRUE(report.anomaly_detected);
    EXPECT_FLOAT_EQ(report.ai_score, score);
}

TEST_F(AnomalyDetectorTest, NegativeScoreDoesNotTriggerAlarm) {
    SetMockSensorData(22.0f, 40.0f, 1013.0f);
    SetMockNpuScore(-0.10f);

    AnomalyReport report = detector_->AnalyzeData();

    EXPECT_FALSE(report.anomaly_detected);
    EXPECT_FLOAT_EQ(report.ai_score, -0.10f);
}

TEST_F(AnomalyDetectorTest, EvaluatesConsecutiveReadingsIndependently) {
    EXPECT_CALL(mock_sensor_, ReadData())
        .WillOnce(Return(SensorData{22.0f, 40.0f, 1013.0f}))
        .WillOnce(Return(SensorData{60.0f, 20.0f, 1013.0f}));
    EXPECT_CALL(mock_npu_, RunInference(_)).WillOnce(Return(0.79f)).WillOnce(Return(0.81f));

    AnomalyReport normal = detector_->AnalyzeData();
    AnomalyReport anomaly = detector_->AnalyzeData();

    EXPECT_FALSE(normal.anomaly_detected);
    EXPECT_FLOAT_EQ(normal.ai_score, 0.79f);
    EXPECT_TRUE(anomaly.anomaly_detected);
    EXPECT_FLOAT_EQ(anomaly.ai_score, 0.81f);
}

TEST_F(AnomalyDetectorTest, NaNScoreDoesNotTriggerAlarm) {
    SetMockSensorData(22.0f, 40.0f, 1013.0f);
    SetMockNpuScore(std::numeric_limits<float>::quiet_NaN());

    AnomalyReport report = detector_->AnalyzeData();

    EXPECT_FALSE(report.anomaly_detected);
    EXPECT_TRUE(std::isnan(report.ai_score));
}
