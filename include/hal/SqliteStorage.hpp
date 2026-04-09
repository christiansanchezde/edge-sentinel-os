#pragma once
#include "IDatabase.hpp"
#include <sqlite3.h>
#include <string>
#include <mutex>

namespace edge::hal {

class SqliteStorage : public IDatabase {
private:
    sqlite3* db_;
    std::string db_path_;
    std::mutex db_mutex_; // SQLite is thread-safe, but a mutex ensures our queries don't interleave

    bool ExecuteQuery(const std::string& query);

public:
    explicit SqliteStorage(const std::string& db_path);
    ~SqliteStorage() override;

    bool Init() override;
    bool LogReading(const SensorData& data, float anomaly_score) override;
    bool LogSystemMessage(const std::string& level, const std::string& tag, const std::string& message) override;
};

} // namespace edge::hal