#pragma once
#include <cstdint>

class Joypad{
public:
    Joypad() = default;

    uint8_t read_byte() const;
    void write_byte(uint8_t value);

    void key_pressed(uint8_t button);
    void key_released(uint8_t button);
private:

    // Recordar que los botones son activos por bajo.

    uint8_t action_buttons = 0x0F;    // Start, Select, B, A
    uint8_t direction_buttons = 0x0F; // Down, Up, Left, Right
    uint8_t select_mask = 0x30;       // Bits 4 y 5 del registro 0xFF00
};
