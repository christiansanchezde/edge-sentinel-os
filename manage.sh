#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

COMMAND=$1

case "$COMMAND" in
    "build")
        echo "🔨 Building project locally (WSL)..."
        # Generate build files and compile using all available CPU cores
        cmake -B build -S .
        cmake --build build -j$(nproc)
        echo "✅ Build complete."
        ;;
    "test")
        echo "🧪 Running unit tests locally..."
        # If the build directory doesn't exist, build it first
        if [ ! -d "build" ]; then
            $0 build
        fi
        ./build/tests/unit_tests
        ;;
    "clean")
        echo "🧹 Cleaning build directory..."
        rm -rf build
        echo "✅ Clean complete."
        ;;
    "all")
        $0 clean
        $0 build
        $0 test
        ;;
    *)
        echo "Usage: ./manage.sh {build|test|clean|all}"
        echo ""
        echo "Commands:"
        echo "  build   : Compiles the application and tests locally."
        echo "  test    : Runs the Google Test suite locally."
        echo "  clean   : Removes the local build directory."
        echo "  all     : Runs clean, build, and test in sequence."
        exit 1
        ;;
esac