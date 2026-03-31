#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "hal/ISensor.hpp"
#include "AnomalyDetector.hpp" // Now including our refactored class

using namespace edge::hal;
using namespace edge::app;
using ::testing::Return;

// 1. THE MOCK
class MockSensor : public ISensor {
public:
    MOCK_METHOD(bool, Init, (), (override));
    MOCK_METHOD(SensorData, ReadData, (), (override));
};

// 2. THE UNIT TESTS
TEST(AnomalyDetectorTest, DetectsFireWhenTemperatureIsHigh) {
    MockSensor fake_sensor;
    EXPECT_CALL(fake_sensor, ReadData())
        .WillOnce(Return(SensorData{60.0f, 20.0f, 1013.0f}));

    AnomalyDetector detector(fake_sensor);
    EXPECT_TRUE(detector.CheckForFire());
}

TEST(AnomalyDetectorTest, NormalConditionsNoFire) {
    MockSensor fake_sensor;
    EXPECT_CALL(fake_sensor, ReadData())
        .WillOnce(Return(SensorData{22.0f, 40.0f, 1013.0f}));

    AnomalyDetector detector(fake_sensor);
    EXPECT_FALSE(detector.CheckForFire());
}