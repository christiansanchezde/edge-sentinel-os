# 1. Create a library for our core application logic
add_library(app_core 
    AnomalyDetector.cpp
    # (Future files like BME280.cpp will go here)
)

# 2. Create the main executable
add_executable(${PROJECT_NAME} main.cpp)

# 3. Link the core library to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE app_core)