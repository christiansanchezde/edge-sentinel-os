cmake_minimum_required(VERSION 3.14)
project(edge-sentinel-os VERSION 1.0.0 LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# Add the main executable
add_executable(${PROJECT_NAME} src/main.cpp)

# (Later link SQLite, RKNN, and I2C libraries here)