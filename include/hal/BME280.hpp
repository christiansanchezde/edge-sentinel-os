#pragma once
#include "ISensor.hpp"
#include <string>
#include <cstdint>

namespace edge::hal {

// Structure to hold the factory calibration data
struct BME280Calibration {
    uint16_t dig_T1; int16_t dig_T2; int16_t dig_T3;
    uint16_t dig_P1; int16_t dig_P2; int16_t dig_P3; int16_t dig_P4; 
    int16_t dig_P5; int16_t dig_P6; int16_t dig_P7; int16_t dig_P8; int16_t dig_P9;
    uint8_t  dig_H1; int16_t dig_H2; uint8_t  dig_H3; int16_t dig_H4; 
    int16_t  dig_H5; int8_t  dig_H6;
};

class BME280 : public ISensor {
private:
    std::string i2c_bus_;
    int i2c_address_;
    int file_descriptor_;
    BME280Calibration calib_data_;
    int32_t t_fine_; // Global variable used in temperature compensation to calculate humidity/pressure

    bool WriteRegister(uint8_t reg, uint8_t value);
    bool ReadCalibrationData();

public:
    BME280(const std::string& bus, int address);
    ~BME280() override;

    bool Init() override;
    SensorData ReadData() override;
};

} // namespace edge::hal