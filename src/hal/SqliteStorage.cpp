#include "hal/SqliteStorage.hpp"
#include "Logger.hpp"

namespace edge::hal {

SqliteStorage::SqliteStorage(const std::string& db_path) : db_(nullptr), db_path_(db_path) {}

SqliteStorage::~SqliteStorage() {
    if (db_) {
        sqlite3_close(db_);
        LOG_INFO("[DB] SQLite database closed safely.");
    }
}

bool SqliteStorage::ExecuteQuery(const std::string& query) {
    char* error_message = nullptr;
    int exit_code = sqlite3_exec(db_, query.c_str(), nullptr, 0, &error_message);
    
    if (exit_code != SQLITE_OK) {
        LOG_ERROR("[DB] SQL Error: " << error_message);
        sqlite3_free(error_message);
        return false;
    }
    return true;
}

bool SqliteStorage::Init() {
    int exit_code = sqlite3_open(db_path_.c_str(), &db_);
    
    if (exit_code != SQLITE_OK) {
        LOG_ERROR("[DB] Failed to open database: " << sqlite3_errmsg(db_));
        return false;
    }

    LOG_INFO("[DB] Connected to SQLite database at " << db_path_);

    // Create the table if it doesn't exist
    // We use CURRENT_TIMESTAMP to automatically record when the data was saved
    std::string create_table_sql = 
        "CREATE TABLE IF NOT EXISTS sensor_logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "temperature REAL, "
        "humidity REAL, "
        "pressure REAL, "
        "anomaly_score REAL"
        ");";

    return ExecuteQuery(create_table_sql);
}

bool SqliteStorage::LogReading(const SensorData& data, float anomaly_score) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    // Construct the SQL INSERT statement
    std::string insert_sql = 
        "INSERT INTO sensor_logs (temperature, humidity, pressure, anomaly_score) VALUES (" +
        std::to_string(data.temperature) + ", " +
        std::to_string(data.humidity) + ", " +
        std::to_string(data.pressure) + ", " +
        std::to_string(anomaly_score) + ");";

    bool success = ExecuteQuery(insert_sql);
    if (success) {
        LOG_DEBUG("[DB] Logged reading to database.");
    }
    return success;
}

} // namespace edge::hal