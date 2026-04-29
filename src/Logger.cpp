#include "Logger.hpp"
#include <iostream>

namespace edge::core {

Logger::Logger() {
    running_ = true;
    worker_thread_ = std::thread(&Logger::ProcessLogs, this);
}

Logger::~Logger() {
    running_ = false;
    log_queue_.Shutdown();
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void Logger::Log(LogLevel level, const std::string& file, int line, const std::string& message) {
    // 1. Minimum work on the main thread: just extract the filename/tag
    std::string filename = file.substr(file.find_last_of("/\\") + 1);
    std::string tag = filename + ":" + std::to_string(line);

    // 2. Hand off to the background worker
    log_queue_.Push({level, tag, message});
}

void Logger::ProcessLogs() {
    LogEntry entry;
    while (log_queue_.Pop(entry)) {
        std::string level_str = LevelToString(entry.level);
        
        // 1. Output to Terminal (Background thread handles the slow I/O)
        std::ostream& os = (entry.level >= LogLevel::ERROR) ? std::cerr : std::cout;
        os << "[" << level_str << "] [" << entry.tag << "] " << entry.message << std::endl;

        // 2. Output to Database
        if (db_) {
            // Clean the string for DB (e.g. "INFO " -> "INFO")
            std::string clean_level = level_str;
            if (!clean_level.empty() && clean_level.back() == ' ') clean_level.pop_back();
            
            db_->LogSystemMessage(clean_level, entry.tag, entry.message);
        }
    }
}

void Logger::PruneOldData(int days_to_keep) {
    if (!db_) return;
    
    // This runs on whichever thread calls it (usually during startup)
    // We'll assume your IDatabase has a generic Execute method for raw SQL
    std::string query = "DELETE FROM logs WHERE timestamp < datetime('now', '-" + 
                        std::to_string(days_to_keep) + " days');";
    
    // Note: You'll need to add a method like ExecuteRawQuery to your IDatabase interface
    // For now, we can log that the janitor is starting
    this->Log(LogLevel::INFO, __FILE__, __LINE__, "Janitor: Cleaning logs older than " + std::to_string(days_to_keep) + " days...");
}

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO ";
        case LogLevel::WARN:     return "WARN ";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

}  // namespace edge::core