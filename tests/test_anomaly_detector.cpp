#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "hal/ISensor.hpp"
#include "hal/INpuModel.hpp"
#include "AnomalyDetector.hpp"

using namespace edge::hal;
using namespace edge::app;
using ::testing::Return;
using ::testing::_; // Allows us to use wildcards in EXPECT_CALL

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
// 2. THE UNIT TESTS
// ==========================================

TEST(AnomalyDetectorTest, TriggersAlarmOnHighAnomalyScore) {
    // Arrange
    MockSensor fake_sensor;
    MockNpuModel fake_npu;

    // Sensor reads hot data
    EXPECT_CALL(fake_sensor, ReadData())
        .WillOnce(Return(SensorData{60.0f, 20.0f, 1013.0f}));

    // NPU analyzes it and returns a 95% anomaly score
    EXPECT_CALL(fake_npu, RunInference(_))
        .WillOnce(Return(0.95f));

    // Inject both dependencies!
    AnomalyDetector detector(fake_sensor, fake_npu);

    // Act & Assert (Should trigger because 0.95 > 0.80)
    EXPECT_TRUE(detector.CheckForFire());
}

TEST(AnomalyDetectorTest, NoAlarmOnNormalScore) {
    // Arrange
    MockSensor fake_sensor;
    MockNpuModel fake_npu;

    // Sensor reads normal data
    EXPECT_CALL(fake_sensor, ReadData())
        .WillOnce(Return(SensorData{22.0f, 40.0f, 1013.0f}));

    // NPU analyzes it and returns a 10% anomaly score
    EXPECT_CALL(fake_npu, RunInference(_))
        .WillOnce(Return(0.10f));

    AnomalyDetector detector(fake_sensor, fake_npu);

    // Act & Assert (Should NOT trigger because 0.10 < 0.80)
    EXPECT_FALSE(detector.CheckForFire());
}