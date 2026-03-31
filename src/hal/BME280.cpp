#include "hal/BME280.hpp"
#include <iostream>
#include <fcntl.h>      // File control definitions (open)
#include <unistd.h>     // UNIX standard function definitions (read, write, close)
#include <sys/ioctl.h>  // IOCTL definitions
#include <linux/i2c-dev.h> // I2C Linux definitions

namespace edge::hal {

BME280::BME280(const std::string& bus, int address) 
    : i2c_bus(bus), i2c_address(address), file_descriptor(-1) {}

BME280::~BME280() {
    if (file_descriptor >= 0) {
        close(file_descriptor);
        std::cout << "[BME280] I2C bus closed safely.\n";
    }
}

bool BME280::init() {
    // 1. Open the I2C bus device file
    file_descriptor = open(i2c_bus.c_str(), O_RDWR);
    if (file_descriptor < 0) {
        std::cerr << "[ERROR] Failed to open the I2C bus: " << i2c_bus << "\n";
        return false;
    }

    // 2. Specify the address of the I2C slave (the BME280)
    if (ioctl(file_descriptor, I2C_SLAVE, i2c_address) < 0) {
        std::cerr << "[ERROR] Failed to acquire bus access to address 0x" 
                  << std::hex << i2c_address << "\n";
        return false;
    }

    // 3. Wake up the sensor (Write to Ctrl_Meas register to set normal mode)
    if (!writeRegister(0xF4, 0x27)) {
        std::cerr << "[ERROR] Failed to initialize BME280.\n";
        return false;
    }

    std::cout << "[BME280] Initialization successful on " << i2c_bus << " at 0x" 
              << std::hex << i2c_address << std::dec << "\n";
    return true;
}

/**
 * Helper method to write a byte to a specific register on the BME280 sensor.
 */
bool BME280::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    if (write(file_descriptor, buffer, 2) != 2) {
        return false;
    }
    return true;
}

/**
 * Reads data from the BME280 sensor.
 * @return A SensorData struct containing the temperature, humidity, and pressure readings.
 */
SensorData BME280::readData() {
    SensorData data = {0.0f, 0.0f, 0.0f};

    // The BME280 data registers start at 0xF7. 
    // We write the register address we want to read from first.
    uint8_t reg = 0xF7;
    if (write(file_descriptor, &reg, 1) != 1) {
        std::cerr << "[ERROR] Failed to write to data register.\n";
        return data;
    }

    // Read 8 bytes of raw data (Pressure, Temp, Humidity)
    uint8_t buffer[8] = {0};
    if (read(file_descriptor, buffer, 8) != 8) {
        std::cerr << "[ERROR] Failed to read data bytes.\n";
        return data;
    }

    // --- MOCK CALCULATION FOR PoC ---
    // In a production app, you apply the Bosch calibration matrix here.
    // For this demonstration, we are parsing raw bytes into readable mock numbers
    // to prove the I2C pipeline successfully pulled data from the sensor.
    
    // Simulate ~25C temp based on raw byte fluctuation
    data.temperature = 20.0f + (buffer[3] % 10); 
    data.humidity = 40.0f + (buffer[6] % 10);
    data.pressure = 1000.0f + (buffer[0] % 20);

    return data;
}

} // namespace edge::hal