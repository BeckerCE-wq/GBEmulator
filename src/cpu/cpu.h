#pragma once

#include "bus/bus.h"

#include <stdint.h>

class Bus;

class CPU {
public:
    CPU(Bus* bus);
    void step();

private:

    Bus* bus;

    uint8_t read_byte(uint16_t address);
    void write_byte(uint16_t address, uint8_t value);

    // 1. Registros de 8 bits 
    uint8_t a, f; // Acumulador y Banderas (Flags)
    uint8_t b, c;
    uint8_t d, e;
    uint8_t h, l;

    // 2. Registros de 16 bits especiales
    uint16_t sp;  // Stack Pointer
    uint16_t pc;  // Program Counter

    // 3. Modos / Interrupciones
    bool ime;     // Interrupt Master Enable

    // 4. Etapas del ciclo
    uint8_t fetch();
    void decode_and_execute(uint8_t opcode);

    // 5. Banderas (El registro 'f' solo usa 4 banderas)
    void set_flag_z(bool val); // Zero flag
    void set_flag_n(bool val); // Subtraction flag
    void set_flag_h(bool val); // Half Carry flag
    void set_flag_c(bool val); // Carry flag

    bool get_flag_z() const;
    bool get_flag_n() const;
    bool get_flag_h() const;
    bool get_flag_c() const;

    // Helpers para operaciones de doble registro.

    inline uint16_t get_bc() const {return (b << 8) | c; }
    inline uint16_t get_de() const {return (d << 8) | e; }
    inline uint16_t get_hl() const {return (h << 8) | l; }
    inline uint16_t get_af() const { return (a << 8) | f; }

    inline void set_bc(uint16_t val){b = val >> 8; c = val & 0xFF; }
    inline void set_de(uint16_t val){d = val >> 8; e = val & 0xFF; }
    inline void set_hl(uint16_t val){h = val >> 8; l = val & 0xFF; }
    inline void set_af(uint16_t val){a = val >> 8; f = val & 0xF0;} // Solo se usan los primeros 4 bits de f y el resto siempre deben ser ceros.

};
