# System Architecture

## Core Philosophy
The system relies on a strictly layered architecture to separate business logic from hardware specifics, allowing for robust unit testing via Mock objects.

## Layers
1. **Application Layer (C++):** Orchestrates data flow, manages thread pools, and handles timing loops.
2. **AI Inference Layer (C++):** Interfaces with the NPU using the RKNN C API to analyze data streams.
3. **Hardware Abstraction Layer (HAL) (C++):** - `ISensor`: Interface for BME280.
   - `IGPIO`: Interface for LED/Actuator control.
   - `IDatabase`: Interface for SQLite.
4. **Presentation Layer (Python):** A lightweight web server reading the SQLite DB to serve dashboards.