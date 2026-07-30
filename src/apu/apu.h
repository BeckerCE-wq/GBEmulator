#pragma once
#include <cstdint>
#include <vector>

class APU{

public:
    APU() = default;

    void tick(uint8_t m_cycles);

    uint8_t read_byte(uint16_t address) const { return 0xFF; }
    void write_byte(uint16_t address, uint8_t value) {}

    std::vector<float> get_audio_buffer();
private:

    std::vector<float> audio_buffer;
};
