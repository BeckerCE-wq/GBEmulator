#pragma once
#include <cstdint>
#include <vector>
#include <string>

class Cartridge{
public:
    Cartridge() = default;

    bool load_rom(const std::string& filepath);

    uint8_t read_byte(uint16_t address) const;
    void write_byte(uint16_t address, uint8_t value);

private:
    std::vector<uint8_t> rom_data; // 0x0000 - 0x7FFF
};
