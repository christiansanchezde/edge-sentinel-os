#include "Logger.hpp"

namespace edge::core {

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

void Logger::SetLevelFromString(const std::string& levelStr) {
    if (levelStr == "DEBUG") current_level_ = LogLevel::DEBUG;
    else if (levelStr == "INFO")  current_level_ = LogLevel::INFO;
    else if (levelStr == "WARN")  current_level_ = LogLevel::WARN;
    else if (levelStr == "ERROR") current_level_ = LogLevel::ERROR;
    else if (levelStr == "FATAL") current_level_ = LogLevel::FATAL;
    else current_level_ = LogLevel::INFO; // Default fallback
}

void Logger::Log(LogLevel level, const std::string& file, int line, const std::string& message) {
    // Only print if the level is high enough
    if (level < current_level_) return;

    // Extract just the filename from the full path
    std::string filename = file.substr(file.find_last_of("/\\") + 1);

    // Lock the mutex so threads don't scramble the text
    std::lock_guard<std::mutex> lock(log_mutex_);

    // Choose output stream (Errors go to cerr, everything else to cout)
    std::ostream& os = (level >= LogLevel::ERROR) ? std::cerr : std::cout;

    // Format: [LEVEL] [File.cpp:Line] Message
    os << "[" << LevelToString(level) << "] "
       << "[" << filename << ":" << line << "] " 
       << message << "\n";
}

} // namespace edge::core