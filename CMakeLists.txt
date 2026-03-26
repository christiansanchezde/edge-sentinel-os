cmake_minimum_required(VERSION 3.14)
project(edge-sentinel-os VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Helps VS Code IntelliSense

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Fetch Google Test & Google Mock
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0  # Updated to the latest release
)
FetchContent_MakeAvailable(googletest)

# Add the main executable
add_executable(${PROJECT_NAME} src/main.cpp)

# --- TESTING ---
enable_testing()
add_executable(unit_tests tests/test_anomaly_detector.cpp)
# Link GTest, GMock, and a main function automatically
target_link_libraries(unit_tests PRIVATE GTest::gtest_main GTest::gmock)