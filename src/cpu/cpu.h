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

    
};
