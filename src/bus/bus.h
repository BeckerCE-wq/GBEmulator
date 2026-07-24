#pragma once
#include <stdint.h>

class Bus {
public:
    Bus();
    
    uint8_t read_byte(uint16_t address);
    void write_byte(uint16_t address, uint8_t value);

private:
    uint8_t memory[0x10000]; 
};
