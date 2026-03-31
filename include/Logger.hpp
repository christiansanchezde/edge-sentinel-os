#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <sstream>

namespace edge::core {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Logger {
private:
    LogLevel currentLevel;
    std::mutex logMutex;

    Logger() : currentLevel(LogLevel::INFO) {}

    // Helper to convert enum to string
    std::string levelToString(LogLevel level);

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Set the minimum level to display
    void setLevel(LogLevel level) { currentLevel = level; }
    void setLevelFromString(const std::string& levelStr);

    // The core logging function
    void log(LogLevel level, const std::string& file, int line, const std::string& message);
};

} // namespace edge::core

// ==========================================
// SERILOG-STYLE MACROS
// ==========================================
// These macros automatically inject the __FILE__ and __LINE__ variables
#define LOG_DEBUG(msg) do { std::ostringstream oss; oss << msg; edge::core::Logger::getInstance().log(edge::core::LogLevel::DEBUG, __FILE__, __LINE__, oss.str()); } while(0)
#define LOG_INFO(msg)  do { std::ostringstream oss; oss << msg; edge::core::Logger::getInstance().log(edge::core::LogLevel::INFO,  __FILE__, __LINE__, oss.str()); } while(0)
#define LOG_WARN(msg)  do { std::ostringstream oss; oss << msg; edge::core::Logger::getInstance().log(edge::core::LogLevel::WARN,  __FILE__, __LINE__, oss.str()); } while(0)
#define LOG_ERROR(msg) do { std::ostringstream oss; oss << msg; edge::core::Logger::getInstance().log(edge::core::LogLevel::ERROR, __FILE__, __LINE__, oss.str()); } while(0)
#define LOG_FATAL(msg) do { std::ostringstream oss; oss << msg; edge::core::Logger::getInstance().log(edge::core::LogLevel::FATAL, __FILE__, __LINE__, oss.str()); } while(0)