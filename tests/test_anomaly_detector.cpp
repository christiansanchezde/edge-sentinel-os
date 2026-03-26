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
    MOCK_METHOD(bool, init, (), (override));
    MOCK_METHOD(SensorData, readData, (), (override));
};

// 2. THE UNIT TESTS
TEST(AnomalyDetectorTest, DetectsFireWhenTemperatureIsHigh) {
    MockSensor fakeSensor;
    EXPECT_CALL(fakeSensor, readData())
        .WillOnce(Return(SensorData{60.0f, 20.0f, 1013.0f}));

    AnomalyDetector detector(fakeSensor);
    EXPECT_TRUE(detector.checkForFire());
}

TEST(AnomalyDetectorTest, NormalConditionsNoFire) {
    MockSensor fakeSensor;
    EXPECT_CALL(fakeSensor, readData())
        .WillOnce(Return(SensorData{22.0f, 40.0f, 1013.0f}));

    AnomalyDetector detector(fakeSensor);
    EXPECT_FALSE(detector.checkForFire());
}