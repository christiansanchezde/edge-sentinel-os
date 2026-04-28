#pragma once
#include <mutex>
#include <sstream>
#include <string>

#include "hal/IDatabase.hpp"

// Compile-time log levels
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_CRITICAL 4

// Fallback if not provided by CMake
#ifndef ACTIVE_LOG_LEVEL
#define ACTIVE_LOG_LEVEL LOG_LEVEL_INFO
#endif

namespace edge::core {

enum class LogLevel { DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3, CRITICAL = 4 };

class Logger {
   private:
    std::mutex log_mutex_;
    edge::hal::IDatabase* db_ = nullptr;

    Logger() = default;
    std::string LevelToString(LogLevel level);

   public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    void SetDatabase(edge::hal::IDatabase* db) {
        db_ = db;
    }
    void Log(LogLevel level, const std::string& file, int line, const std::string& message);
};

}  // namespace edge::core

// ==========================================
// ZERO-OVERHEAD MACROS
// ==========================================
#if ACTIVE_LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(msg)                                                                         \
    do {                                                                                       \
        std::ostringstream oss;                                                                \
        oss << msg;                                                                            \
        edge::core::Logger::GetInstance().Log(edge::core::LogLevel::DEBUG, __FILE__, __LINE__, \
                                              oss.str());                                      \
    } while (0)
#else
#define LOG_DEBUG(msg) \
    do {               \
    } while (0)
#endif

#if ACTIVE_LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(msg)                                                                         \
    do {                                                                                      \
        std::ostringstream oss;                                                               \
        oss << msg;                                                                           \
        edge::core::Logger::GetInstance().Log(edge::core::LogLevel::INFO, __FILE__, __LINE__, \
                                              oss.str());                                     \
    } while (0)
#else
#define LOG_INFO(msg) \
    do {              \
    } while (0)
#endif

#define LOG_WARN(msg)                                                                         \
    do {                                                                                      \
        std::ostringstream oss;                                                               \
        oss << msg;                                                                           \
        edge::core::Logger::GetInstance().Log(edge::core::LogLevel::WARN, __FILE__, __LINE__, \
                                              oss.str());                                     \
    } while (0)
#define LOG_ERROR(msg)                                                                         \
    do {                                                                                       \
        std::ostringstream oss;                                                                \
        oss << msg;                                                                            \
        edge::core::Logger::GetInstance().Log(edge::core::LogLevel::ERROR, __FILE__, __LINE__, \
                                              oss.str());                                      \
    } while (0)
#define LOG_CRITICAL(msg)                                                                         \
    do {                                                                                          \
        std::ostringstream oss;                                                                   \
        oss << msg;                                                                               \
        edge::core::Logger::GetInstance().Log(edge::core::LogLevel::CRITICAL, __FILE__, __LINE__, \
                                              oss.str());                                         \
    } while (0)