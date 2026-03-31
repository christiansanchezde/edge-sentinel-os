#include "Logger.hpp"

namespace edge::core {

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

void Logger::setLevelFromString(const std::string& levelStr) {
    if (levelStr == "DEBUG") currentLevel = LogLevel::DEBUG;
    else if (levelStr == "INFO")  currentLevel = LogLevel::INFO;
    else if (levelStr == "WARN")  currentLevel = LogLevel::WARN;
    else if (levelStr == "ERROR") currentLevel = LogLevel::ERROR;
    else if (levelStr == "FATAL") currentLevel = LogLevel::FATAL;
    else currentLevel = LogLevel::INFO; // Default fallback
}

void Logger::log(LogLevel level, const std::string& file, int line, const std::string& message) {
    // Only print if the level is high enough
    if (level < currentLevel) return;

    // Extract just the filename from the full path
    std::string filename = file.substr(file.find_last_of("/\\") + 1);

    // Lock the mutex so threads don't scramble the text
    std::lock_guard<std::mutex> lock(logMutex);
    
    // Choose output stream (Errors go to cerr, everything else to cout)
    std::ostream& os = (level >= LogLevel::ERROR) ? std::cerr : std::cout;

    // Format: [LEVEL] [File.cpp:Line] Message
    os << "[" << levelToString(level) << "] "
       << "[" << filename << ":" << line << "] " 
       << message << "\n";
}

} // namespace edge::core