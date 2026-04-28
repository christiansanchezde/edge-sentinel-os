#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

BUILD_DIR="build"

function clean() {
    echo "[System] Cleaning build directory..."
    rm -rf ${BUILD_DIR}
    echo "[System] Clean complete."
}

function build() {
    echo "[System] Configuring and building Edge Sentinel OS..."
    mkdir -p ${BUILD_DIR}
    cd ${BUILD_DIR}
    
    # Run CMake and Make using all available CPU cores
    cmake ..
    make -j$(nproc)
    
    cd ..
    echo "[System] Build successful."
}

function run_tests() {
    echo "=========================================="
    echo " 🧪 RUNNING TEST SUITE                    "
    echo "=========================================="
    
    if [ ! -f "./${BUILD_DIR}/tests/unit_tests" ]; then
        echo "[Error] Test binary not found. Please run './manage.sh build' first."
        exit 1
    fi

    # Execute the Google Test binary
    ./${BUILD_DIR}/tests/unit_tests
}

function run_stack() {
    echo "=========================================="
    echo " 🚀 STARTING EDGE SENTINEL OS PIPELINE    "
    echo "=========================================="

    # Ask for password UPFRONT so it doesn't freeze in the background
    sudo -v

    # 1. Ensure the app is built
    if [ ! -f "./${BUILD_DIR}/src/edge-sentinel-os" ]; then
        echo "[System] Binary not found. Building first..."
        build
    fi

    # 2. Trap Ctrl+C (SIGINT) to cleanly kill background processes
    # This prevents ghost processes from hanging around and hogging port 5000
    trap 'echo -e "\n[System] Shutting down Edge Sentinel OS..."; kill $FLASK_PID 2>/dev/null; sudo kill $CPP_PID 2>/dev/null; exit 0' SIGINT

    # 3. Launch the Web UI in the background
    echo "[System] Starting Web Dashboard on Port 5000..."
    python3 web_ui/backend/app.py &
    FLASK_PID=$!

    # Give Flask a second to spin up
    sleep 2 

    # 4. Launch the C++ Core in the background (Requires sudo for hardware/I2C access)
    echo "[System] Starting C++ Core Engine..."
    sudo ./${BUILD_DIR}/src/edge-sentinel-os &
    CPP_PID=$!

    echo "=========================================="
    echo " ✅ SYSTEM ONLINE - Press [Ctrl+C] to stop"
    echo "=========================================="

    # Wait forever until Ctrl+C is pressed
    wait
}

function format() {
    echo "[System] Formatting code with clang-format..."
    find src include tests -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
    echo "[System] Done."
}

# --- Command Line Argument Parsing ---
case "$1" in
    build)
        build
        ;;
    test)
        run_tests
        ;;
    clean)
        clean
        ;;
    all)
        clean
        build
        run_tests
        ;;
    run)
        run_stack
        ;;
    format)
        format
        ;;
    *)
        echo "Usage: $0 {build|test|clean|all|run}"
        echo "  build : Compiles the C++ source code"
        echo "  test  : Runs the Google Test suite"
        echo "  clean : Removes the build directory"
        echo "  all   : Cleans, builds, and runs tests"
        echo "  run   : Launches both the C++ OS and the Python Web UI"
        echo "  format: Formats code using clang-format (.clang-format file)"
        exit 1
        ;;
esac