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
    bool halted;
    bool stopped;

    // 4. Etapas del ciclo
    uint8_t fetch();
    uint16_t fetch_16();
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
    inline uint16_t get_af() const {return (a << 8) | f; }

    inline void set_bc(uint16_t val){b = val >> 8; c = val & 0xFF; }
    inline void set_de(uint16_t val){d = val >> 8; e = val & 0xFF; }
    inline void set_hl(uint16_t val){h = val >> 8; l = val & 0xFF; }
    inline void set_af(uint16_t val){a = val >> 8; f = val & 0xF0;} // Solo se usan los primeros 4 bits de f y el resto siempre deben ser ceros.

    // Instruciones...

    void op_nop();

    // BLOQUE LD
    
    void op_ld_r8_imm8(uint8_t& reg);
    void op_ld_r8_r8(uint8_t& reg1, uint8_t& reg2);
    void op_ld_r8_r16mem(uint8_t& reg, uint16_t address);

    void op_ld_r16_imm16(uint8_t& reg_high, uint8_t& reg_low);
    void op_ld_r16mem_r8(uint16_t address, uint8_t& reg);
    void op_ld_r16mem_imm8(uint16_t address);
    

    void op_ld_imm16mem_sp();
    void op_ld_hl_inc_a();
    void op_ld_a_hl_inc();

    void op_ld_hl_dec_a();
    void op_ld_a_hl_dec();
    void op_ldh_n16_amem(uint8_t value);
    void op_ldh_amem_n16(uint8_t value);

    void op_ld_hl_sp_e8();
    void op_ld_sp_hl();
    // BLOQUE INC y DEC
    void op_inc_r8(uint8_t& reg);
    void op_dec_r8(uint8_t& reg);

    void op_inc_r16(uint8_t& reg_high, uint8_t& reg_low);
    void op_dec_r16(uint8_t& reg_high, uint8_t& reg_low);
    void op_inc_r16mem(uint16_t address);
    void op_dec_r16mem(uint16_t address);
    // BLOQUE LÓGICO

    void op_or_r8(uint8_t reg);
    void op_and_r8(uint8_t reg);
    void op_xor_r8(uint8_t reg);

    // BLOQUE ARITMETICO

    void op_add_r8(uint8_t reg);
    void op_sub_r8(uint8_t reg);
    void op_addc_r8(uint8_t reg);
    void op_sbc_r8(uint8_t reg);
    void op_cp_r8(uint8_t reg);

    void op_add_r16(uint16_t reg);

    void op_add_sp_e8();
    // BLOQUE POP/PUSH

    void op_pop_r16(uint8_t& reg_high, uint8_t& reg_low);
    void op_push_r16(uint8_t& reg_high, uint8_t& reg_low);

    // NO SÉ QUÉ ES ESTO
    void op_halt();
    void op_stop();

    // ROTACIONES
    void op_rlca();
    void op_rla();
    void op_rrca();
    void op_rra();

    // SALTOS

    void op_jr_condition_e8(bool condition);
    void op_jp_condition_n16(bool condition);
    void op_jp_hl();
    void op_call_condition_n16(bool condition);
    void op_ret_condition(bool condition);
    void op_reti();
};
