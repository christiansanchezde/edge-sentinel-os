# Introduction

This is a personal project developed during my parental leave. In the limited free time I have between changing diapers and having fun with my children, I am taking the opportunity to explore Edge AI. Let's see how far we can get.

# Edge Sentinel OS

Edge Sentinel OS is a C++ Linux application designed for edge computing and AI inference. Running on an Orange Pi 4 Pro, it collects environmental data (BME280), stores it locally via SQLite, and utilizes the onboard NPU for real-time anomaly detection. A Python-based Web UI provides visualization.

## Hardware Requirements
* Orange Pi 4 Pro (Ubuntu Server 24.04 LTS)
* BME280 I2C Sensor (Temperature, Humidity, Pressure)
* GPIO LEDs

## Features
* **C++ HAL:** Hardware Abstraction Layer for mockable I2C/GPIO interactions.
* **Edge Storage:** Persistent local logging using SQLite3.
* **Edge AI:** NPU-accelerated inference using Rockchip RKNN.
* **Web UI:** Python-based dashboard for data visualization.
* **Quality Assurance:** Built with CMake and tested with Google Test.