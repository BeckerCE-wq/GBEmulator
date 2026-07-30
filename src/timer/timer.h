#pragma once
#include <cstdint>

class Timer{
public:
    Timer() = default;

    void tick(uint8_t m_cycles, bool& interrupt_flag);

    uint8_t read_byte(uint16_t address) const;
    void write_byte(uint16_t address, uint8_t value);

private:

    uint16_t div_counter = 0; // Contador principal interno de 16 bits
    uint8_t tima = 0; // 0xFF05
    uint8_t tma = 0;  // 0xFF06
    uint8_t tac = 0;  // 0xFF07

    bool prev_timer_bit = false;

    bool get_timer_bit() const;
};
