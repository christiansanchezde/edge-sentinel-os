#include "Logger.hpp"
#include <iostream> // Added missing include

namespace edge::core {

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

void Logger::Log(LogLevel level, const std::string& file, int line, const std::string& message) {
    // 1. Format the data
    std::string filename = file.substr(file.find_last_of("/\\") + 1);
    std::string level_str = LevelToString(level);
    std::string tag = filename + ":" + std::to_string(line);

    // Lock mutex for thread safety
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    // 2. Output to Terminal
    std::ostream& os = (level >= LogLevel::ERROR) ? std::cerr : std::cout;
    os << "[" << level_str << "] [" << tag << "] " << message << "\n";

    // 3. Output to Database (If connected)
    if (db_) {
        // Strip the trailing space from "INFO " for clean database storage
        if (level_str.back() == ' ') level_str.pop_back();
        
        db_->LogSystemMessage(level_str, tag, message);
    }
}

} // namespace edge::core