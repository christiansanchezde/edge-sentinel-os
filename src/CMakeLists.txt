# 1. Create a library for our core application logic
add_library(app_core 
    AnomalyDetector.cpp
    hal/BME280.cpp
    hal/RknnModel.cpp
    hal/SimulatedNpuModel.cpp
    ConfigManager.cpp
    Logger.cpp
    hal/SqliteStorage.cpp
)

# Link external libraries to our core library
target_link_libraries(app_core PUBLIC sqlite3)

target_compile_definitions(app_core PRIVATE 
    PROJECT_FW_VERSION="${PROJECT_VERSION}"
    ACTIVE_LOG_LEVEL=LOG_LEVEL_INFO # Log Level: 0=DEBUG, 1=INFO, 2=WARNING, 3=ERROR, 4=CRITICAL
)

# 2. Create the main executable
add_executable(${PROJECT_NAME} main.cpp)

# 3. Link the core library to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE app_core)