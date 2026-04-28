#include "hal/SqliteStorage.hpp"

#include "Logger.hpp"

namespace edge::hal {

SqliteStorage::SqliteStorage(const std::string& db_path) : db_(nullptr), db_path_(db_path) {
}

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
        return false;
    }

    // 1. Enable WAL Mode (Write-Ahead Logging)
    // This is the secret sauce for using SQLite as a bridge between two processes.
    ExecuteQuery("PRAGMA journal_mode=WAL;");

    // 2. Create the tables if they don't exist
    std::string create_tables =
        "CREATE TABLE IF NOT EXISTS sensor_logs ("
        "id INTEGER PRIMARY KEY, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "temperature REAL, humidity REAL, pressure REAL, anomaly_score REAL);"

        "CREATE TABLE IF NOT EXISTS system_logs ("
        "id INTEGER PRIMARY KEY, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "level TEXT, tag TEXT, message TEXT);";

    if (!ExecuteQuery(create_tables)) return false;

    // 3. Maintenance: Apply Retention Policy
    // Every time the app starts, we prune data older than 7 days to save disk space.
    LOG_INFO("Applying database retention policy (7 days)...");

    ExecuteQuery("DELETE FROM sensor_logs WHERE timestamp < datetime('now', '-7 days');");
    ExecuteQuery("DELETE FROM system_logs WHERE timestamp < datetime('now', '-7 days');");

    // Optional: Vacuum the DB to physically reclaim disk space (can be slow on large DBs)
    // ExecuteQuery("VACUUM;");

    return true;
}

bool SqliteStorage::LogReading(const SensorData& data, float anomaly_score) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    // Construct the SQL INSERT statement
    std::string insert_sql =
        "INSERT INTO sensor_logs (temperature, humidity, pressure, anomaly_score) VALUES (" +
        std::to_string(data.temperature) + ", " + std::to_string(data.humidity) + ", " +
        std::to_string(data.pressure) + ", " + std::to_string(anomaly_score) + ");";

    bool success = ExecuteQuery(insert_sql);
    if (success) {
        LOG_DEBUG("[DB] Logged reading to database.");
    }
    return success;
}

bool SqliteStorage::LogSystemMessage(const std::string& level, const std::string& tag,
                                     const std::string& message) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    std::string sql = "INSERT INTO system_logs (level, tag, message) VALUES ('" + level + "', '" +
                      tag + "', '" + message + "');";
    return ExecuteQuery(sql);
}

}  // namespace edge::hal