#pragma once
#include "ISensor.hpp"
#include <string>
#include <cstdint>

namespace edge::hal {

class BME280 : public ISensor {
private:
    std::string i2c_bus;
    int i2c_address;
    int file_descriptor; // Holds the Linux file handle for the I2C bus

    // Helper method to write a byte to a specific register
    bool writeRegister(uint8_t reg, uint8_t value);

public:
    /**
     * @param bus The I2C bus device file (e.g., "/dev/i2c-0")
     * @param address The I2C address of the BME280 sensor (commonly
     */
    BME280(const std::string& bus, int address);
    
    // Destructor ensures the Linux file descriptor is closed
    ~BME280() override;

    // Overriding the ISensor Interface
    bool init() override;
    SensorData readData() override;
};

} // namespace edge::hal