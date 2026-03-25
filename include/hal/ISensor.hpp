#pragma once

namespace edge::hal {

struct SensorData {
    float temperature; // in Celsius
    float humidity;    // in %
    float pressure;    // in hPa
};

class ISensor {
public:
    virtual ~ISensor() = default;
    
    // Initializes the sensor (e.g., sets up I2C)
    virtual bool init() = 0;
    
    // Reads current data from the sensor
    virtual SensorData readData() = 0;
};

} // namespace edge::hal