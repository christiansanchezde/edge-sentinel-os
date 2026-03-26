#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "hal/ISensor.hpp"

using namespace edge::hal;
using ::testing::Return;

// ==========================================
// 1. THE MOCK: A fake sensor for testing
// ==========================================
class MockSensor : public ISensor {
public:
    // We use GMock macros to "override" the virtual functions from ISensor
    MOCK_METHOD(bool, init, (), (override));
    MOCK_METHOD(SensorData, readData, (), (override));
};

// ==========================================
// 2. THE LOGIC: The class we are actually testing
// ==========================================
class AnomalyDetector {
private:
    ISensor& sensor; // Dependency Injection!
public:
    AnomalyDetector(ISensor& s) : sensor(s) {}

    // Simple logic: If temp > 50.0C, return true (Fire detected!)
    bool checkForFire() {
        SensorData currentData = sensor.readData();
        return currentData.temperature > 50.0f;
    }
};

// ==========================================
// 3. THE UNIT TESTS
// ==========================================

TEST(AnomalyDetectorTest, DetectsFireWhenTemperatureIsHigh) {
    // Arrange
    MockSensor fakeSensor;
    
    // We program the fake sensor to return 60.0 C when readData() is called!
    EXPECT_CALL(fakeSensor, readData())
        .WillOnce(Return(SensorData{60.0f, 20.0f, 1013.0f}));

    AnomalyDetector detector(fakeSensor);

    // Act & Assert
    // We expect checkForFire() to return TRUE because 60 > 50
    EXPECT_TRUE(detector.checkForFire());
}

TEST(AnomalyDetectorTest, NormalConditionsNoFire) {
    // Arrange
    MockSensor fakeSensor;
    
    // Program it to return a normal room temperature of 22.0 C
    EXPECT_CALL(fakeSensor, readData())
        .WillOnce(Return(SensorData{22.0f, 40.0f, 1013.0f}));

    AnomalyDetector detector(fakeSensor);

    // Act & Assert
    EXPECT_FALSE(detector.checkForFire());
}