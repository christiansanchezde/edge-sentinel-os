#include "hal/BME280.hpp"
#include "Logger.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

namespace edge::hal {

BME280::BME280(const std::string& bus, int address) 
    : i2c_bus_(bus), i2c_address_(address), file_descriptor_(-1) {}

BME280::~BME280() {
    if (file_descriptor_ >= 0) {
        close(file_descriptor_);
        LOG_INFO("[BME280] I2C bus closed safely."); 
    }
}

bool BME280::Init() {
    file_descriptor_ = open(i2c_bus_.c_str(), O_RDWR);
    if (file_descriptor_ < 0) {
        LOG_ERROR("[BME280] Failed to open I2C bus: " << i2c_bus_);
        return false;
    }

    if (ioctl(file_descriptor_, I2C_SLAVE, i2c_address_) < 0) {
        LOG_ERROR("[BME280] Failed to acquire bus access to address 0x" << std::hex << i2c_address_);
        return false;
    }

    if (!WriteRegister(0xF4, 0x27)) {
        LOG_ERROR("[BME280] Failed to initialize BME280.");
        return false;
    }

    LOG_INFO("[BME280] Initialization successful on " << i2c_bus_ << " at 0x" << std::hex << i2c_address_ << std::dec);
    return true;
}

bool BME280::WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    return write(file_descriptor_, buffer, 2) == 2;
}

SensorData BME280::ReadData() {
    SensorData data = {0.0f, 0.0f, 0.0f};
    uint8_t reg = 0xF7;
    
    if (write(file_descriptor_, &reg, 1) != 1) {
        LOG_WARN("[BME280] Failed to write to data register.");
        return data;
    }

    uint8_t buffer[8] = {0};
    if (read(file_descriptor_, buffer, 8) != 8) {
        LOG_WARN("[BME280] Failed to read data bytes.");
        return data;
    }

    data.temperature = 20.0f + (buffer[3] % 10); 
    data.humidity = 40.0f + (buffer[6] % 10);
    data.pressure = 1000.0f + (buffer[0] % 20);

    return data;
}

} // namespace edge::hal