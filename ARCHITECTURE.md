# System Architecture

## Core Philosophy
The system relies on a strictly layered architecture to separate business logic from hardware specifics, allowing for robust unit testing via Mock objects and execution on both x86 development machines and ARM64 production targets.

## Block Diagram: Separation of Concerns

```mermaid
graph TD
    subgraph Presentation_Layer [Presentation Layer - Python]
        WebUI[Flask Dashboard]
    end

    subgraph App_Layer [Application Layer - C++]
        CoreLogic[AnomalyDetector]
        MainLoop[main.cpp Loop]
        ConfigManager[ConfigManager]
        Logger[Logger]
    end

    subgraph HAL [Hardware Abstraction Layer - Interfaces]
        ISensor[ISensor]
        INpuModel[INpuModel]
        IDatabase[IDatabase]
    end

    subgraph Implementations [Hardware Implementations - Linux/C++]
        BME280[BME280 Driver]
        RKNN[RknnModel / NPU]
        SimNPU[SimulatedNpuModel]
        SQLite[SqliteStorage]
    end

    %% Flow connections
    WebUI -.->|Reads| SQLite
    MainLoop --> CoreLogic
    MainLoop --> ConfigManager
    MainLoop --> Logger
    
    CoreLogic --> ISensor
    CoreLogic --> INpuModel
    
    %% Corrected Implementation Connections
    BME280 -.->|Implements| ISensor
    RKNN -.->|Implements| INpuModel
    SimNPU -.->|Implements| INpuModel
    SQLite -.->|Implements| IDatabase
    
    MainLoop --> IDatabase
```

## Layers
1. **Application Layer (C++):** Orchestrates data flow, manages thread pools, and handles timing loops. Contains the core business logic (`AnomalyDetector`).
2. **AI Inference Layer (C++):** Interfaces with the NPU using the RKNN C API to analyze data streams. Utilizes the Factory Pattern to seamlessly switch between hardware execution and local CPU simulation.
3. **Hardware Abstraction Layer (HAL) (C++):** - `ISensor`: Interface for environmental data collection (Implemented by `BME280`).
   - `IGPIO`: Interface for LED/Actuator control.
   - `INpuModel`: Interface for Edge AI anomaly detection.
   - `IDatabase`: Interface for local persistence (Implemented by `SqliteStorage`).
4. **Presentation Layer (Python):** A lightweight web server reading the SQLite DB to serve dashboards.

## Class Diagram: Dependency Injection & Patterns

```mermaid
classDiagram
    %% Core Logic
    class AnomalyDetector {
        -ISensor& sensor_
        -INpuModel& npu_model_
        +AnomalyDetector(ISensor&, INpuModel&)
        +AnalyzeData() AnomalyReport
    }

    class AnomalyReport {
        <<struct>>
        +bool anomaly_detected
        +float ai_score
    }

    %% Interfaces
    class ISensor {
        <<interface>>
        +Init() bool
        +ReadData() SensorData
    }

    class INpuModel {
        <<interface>>
        +LoadModel(string) bool
        +RunInference(SensorData) float
    }

    class IDatabase {
        <<interface>>
        +Init() bool
        +LogReading(SensorData, float) bool
    }

    %% Concrete Implementations
    class BME280 {
        -string i2c_bus_
        -int i2c_address_
        -BME280Calibration calib_data_
        +Init() bool
        +ReadData() SensorData
    }

    class RknnModel {
        -bool is_loaded_
        +LoadModel(string) bool
        +RunInference(SensorData) float
    }

    class SimulatedNpuModel {
        +LoadModel(string) bool
        +RunInference(SensorData) float
    }

    class SqliteStorage {
        -sqlite3* db_
        +Init() bool
        +LogReading(SensorData, float) bool
    }

    %% Singletons
    class ConfigManager {
        <<Singleton>>
        -AppConfig currentConfig
        +getInstance() ConfigManager&
        +Load(string) bool
    }

    class Logger {
        <<Singleton>>
        -LogLevel currentLevel
        +getInstance() Logger&
        +log(...) void
    }

    %% Relationships
    AnomalyDetector ..> AnomalyReport : Returns
    AnomalyDetector o-- ISensor : Injects
    AnomalyDetector o-- INpuModel : Injects

    ISensor <|.. BME280 : Implements
    INpuModel <|.. RknnModel : Implements
    INpuModel <|.. SimulatedNpuModel : Implements
    IDatabase <|.. SqliteStorage : Implements
```