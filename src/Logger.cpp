#include "Logger.hpp"

namespace edge::core {

/**
 * @class Logger
 * @brief Implementation of the thread-safe Singleton Logger.
 * 
 * This class handles the formatting and output of log messages across the system,
 * ensuring that logs from different threads do not interleave.
 */

/**
 * @brief Converts a LogLevel enum value to a fixed-width string for consistent formatting.
 * 
 * @param level The LogLevel to convert.
 * @return A string representation of the level (e.g., "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL").
 */
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

/**
 * @brief Sets the minimum logging threshold from a string.
 * 
 * Typically used when loading settings from a configuration file.
 * 
 * @param levelStr String representation of the level (e.g., "DEBUG", "WARN").
 *                 Defaults to INFO if the string is unrecognized.
 */
void Logger::SetLevelFromString(const std::string& levelStr) {
    if (levelStr == "DEBUG") current_level_ = LogLevel::DEBUG;
    else if (levelStr == "INFO")  current_level_ = LogLevel::INFO;
    else if (levelStr == "WARN")  current_level_ = LogLevel::WARN;
    else if (levelStr == "ERROR") current_level_ = LogLevel::ERROR;
    else if (levelStr == "FATAL") current_level_ = LogLevel::FATAL;
    else current_level_ = LogLevel::INFO; // Default fallback
}

/**
 * @brief Primary logging function that formats and prints the message.
 * 
 * Filters messages based on the current_level_, extracts the filename from the path,
 * and uses a mutex to ensure thread-safety during console output.
 * 
 * @param level Severity of the log message.
 * @param file The source file path (usually __FILE__).
 * @param line The source line number (usually __LINE__).
 * @param message The message content to be logged.
 */
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