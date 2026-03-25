#include <iostream>
#include "hal/ISensor.hpp"
#include "hal/IGPIO.hpp"

int main() {
    std::cout << "======================================\n";
    std::cout << " Edge Sentinel OS - Initialization\n";
    std::cout << "======================================\n";
    
    std::cout << "[INFO] HAL Interfaces loaded.\n";
    std::cout << "[INFO] Ready for BME280 implementation.\n";

    return 0;
}