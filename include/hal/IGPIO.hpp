#pragma once

namespace edge::hal {

class IGPIO {
public:
    virtual ~IGPIO() = default;
    
    // Initializes the pin (e.g., set as OUTPUT)
    virtual bool Init(int pin) = 0;
    
    // Sets the pin state (true = HIGH, false = LOW)
    virtual void Write(int pin, bool state) = 0;
};

} // namespace edge::hal