#include <iostream>
#include <thread>
#include <chrono>
#include "hal/BME280.hpp"
#include "AnomalyDetector.hpp"

using namespace edge::hal;
using namespace edge::app;

int main() {
    std::cout << "======================================\n";
    std::cout << " Edge Sentinel OS - Starting Up\n";
    std::cout << "======================================\n";

    // 1. Initialize the physical hardware
    // NOTE: Change "/dev/i2c-0" to your actual bus number if it's different!
    BME280 mySensor("/dev/i2c-0", 0x77); 
    
    if (!mySensor.init()) {
        std::cerr << "[FATAL] Sensor initialization failed. Exiting.\n";
        return -1;
    }

    // 2. Inject the hardware into our business logic (Dependency Injection!)
    AnomalyDetector detector(mySensor);

    // 3. Main Application Loop
    std::cout << "\n[INFO] Entering monitoring loop (Press Ctrl+C to exit)...\n";
    
    for (int i = 0; i < 5; ++i) { // Running 5 times for demonstration
        SensorData data = mySensor.readData();
        
        std::cout << "Readings -> Temp: " << data.temperature 
                  << "C, Humidity: " << data.humidity 
                  << "%, Pressure: " << data.pressure << " hPa\n";

        if (detector.checkForFire()) {
            std::cout << "  🚨 ANOMALY DETECTED! TEMPERATURE EXCEEDS THRESHOLD 🚨\n";
        } else {
            std::cout << "  ✅ Conditions normal.\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    std::cout << "\n[INFO] Shutting down Edge Sentinel OS.\n";
    return 0;
}