# Introduction

This is a personal project developed during my parental leave. In the limited free time I have between changing diapers and having fun with my children, I am taking the opportunity to explore Edge AI. Let's see how far we can get.

# Edge Sentinel OS

Edge Sentinel OS is a C++ Linux application designed for edge computing and AI inference. Running on an Orange Pi 4 Pro, it collects environmental data (BME280), stores it locally via SQLite, and utilizes the onboard NPU for real-time anomaly detection. A Python-based Web UI provides visualization.

## Documentation
* [System Architecture](ARCHITECTURE.md) - Details on the C++ HAL, Dependency Injection, and Mermaid diagrams.
* [Hardware Setup](SETUP.md) - Instructions for WSL rsync, SSH keys, and Orange Pi configuration.

## Hardware Requirements
* **Orange Pi 4 Pro** (Ubuntu Server 24.04 LTS)
* **BME280 I2C Sensor** (Temperature, Humidity, Pressure)
* **GPIO LEDs** (Actuators for visual alerts)

## Hardware Schematics
> **TODO:** Insert physical wiring schematic/diagram here.

## Features
* **C++ HAL:** Hardware Abstraction Layer for mockable I2C/GPIO interactions.
* **Edge Storage:** Persistent local logging using SQLite3.
* **Edge AI:** NPU-accelerated inference using Rockchip RKNN (Supports both Simulated and Hardware execution).
* **Web UI:** Python-based dashboard for data visualization.
* **Quality Assurance:** Built with CMake and tested with Google Test.

## Configuration (`config.env`)
The system uses an environment file to dynamically load hardware and application settings without recompiling the C++ binaries. A `config.env` file must exist in the execution directory and supports the following parameters:
```env
I2C_BUS=/dev/i2c-0
I2C_ADDRESS=0x77
LOG_LEVEL=DEBUG
AI_MODE=SIMULATED  # Options: SIMULATED or HARDWARE
```

## Running
To run the current application, you must execute the binary from within the `edge-sentinel-os` root directory. This ensures the executable can locate the `config.env` file; otherwise, it will fall back to hardcoded defaults.

```bash
cd ~/edge-sentinel-os 
sudo ./build/src/edge-sentinel-os
```

## Testing & Local Development
The project uses **Test-Driven Development (TDD)** and adheres to strict Dependency Injection. Because the hardware is abstracted via interfaces (HAL), the core logic can be compiled and tested locally on any host machine without needing the physical Orange Pi or sensors.

* **Framework:** Google Test (`gtest`) and Google Mock (`gmock`).

### Local Testing (WSL / Host)
Use the provided `manage.sh` script to quickly compile and test the business logic on your development machine:
```bash
./manage.sh build   # Compiles the code locally
./manage.sh test    # Runs the unit tests
./manage.sh clean   # Wipes the local build directory
./manage.sh all     # Cleans, builds, and tests in one go
```

### Remote Execution (Orange Pi)
To run the test suite on the actual ARM64 edge target:
1. Trigger the VS Code Build Task (`Ctrl+Shift+B`) to sync and compile on the Pi.
2. SSH into the Pi and execute:
   ```bash
   ~/edge-sentinel-os/build/tests/unit_tests
   ```

## References
* [Orange Pi 4 Pro User Manual](https://drive.google.com/drive/folders/1QxfqUF08jZYx-cK-NrprhHpiG-4puIxd)
