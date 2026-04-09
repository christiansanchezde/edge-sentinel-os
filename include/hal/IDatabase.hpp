#pragma once
#include "ISensor.hpp" // For the SensorData struct
#include <string>

namespace edge::hal {

class IDatabase {
public:
    virtual ~IDatabase() = default;

    // Initialize the database (create the file and tables if they don't exist)
    virtual bool Init() = 0;

    // Log a new reading with its corresponding NPU anomaly score
    virtual bool LogReading(const SensorData& data, float anomaly_score) = 0;

    virtual bool LogSystemMessage(const std::string& level, const std::string& tag, const std::string& message) = 0;
};

} // namespace edge::hal