#pragma once
#include <cstdint>
#include <array>

#include "ppu/ppu.h"
#include "cartridge/cartridge.h"
#include "timer/timer.h"

class Bus {
public:
    Bus();
    
    void insert_cartridge(Cartridge* cart) { cartridge = cart; }

    uint8_t read_byte(uint16_t address);
    void write_byte(uint16_t address, uint8_t value);

private:
    Cartridge* cartridge = nullptr; // ROM y RAM Externa del Cartucho (0x0000 - 0x7FFF y 0xA000 - 0xBFFF)
    PPU ppu; // VRAM (0x8000 - 0x9FFF) y OAM Sprite Memory (0xFE00 - 0xFE9F)
    Timer timer; 

    std::array<uint8_t, 0x2000> wram{}; // Work RAM: 0xC000 - 0xDFFF (8 KiB)
    std::array<uint8_t, 0x007F> hram{}; // High RAM: 0xFF80 - 0xFFFE (127 bytes)

    // REGISTROS MUY IMPORTANTES
    uint8_t interrupt_enable = 0x00; // 0xFFFF (IE)
    uint8_t interrupt_flag   = 0xE0; // 0xFF0F (IF)
    uint8_t serial_data      = 0x00; // 0xFF01 (SB)
    uint8_t serial_control   = 0x7E; // 0xFF02 (SC)
};
