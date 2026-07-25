#include "cpu.h"
#include "bus/bus.h"

#include <iostream>

class Bus;

// Constructor: Estado inicial de la Game Boy al encender
CPU::CPU(Bus* bus){
    this->bus = bus;
    // Al encender la GB (después de la BIOS), los registros arrancan con estos valores:
    a = 0x01;
    f = 0xB0; // Banderas: Z=1, N=0, H=1, C=1
    b = 0x00; c = 0x13;
    d = 0x00; e = 0xD8;
    h = 0x01; l = 0x4D;
    
    sp = 0xFFFE; // La pila empieza al final de la RAM
    pc = 0x0100; // La ROM del juego empieza en la dirección 0x0100 (salteando el logo de Nintendo)
    
    ime = false;
}

uint8_t CPU::read_byte(uint16_t address) {
    return bus->read_byte(address); // La CPU le pide el byte al Bus
}

void CPU::write_byte(uint16_t address, uint8_t value){
    bus->write_byte(address, value);
}

// Flags

void CPU::set_flag_z(bool val){
    if(val) f |= (1 << 7);
    else f &= ~(1 << 7);
}

void CPU::set_flag_n(bool val){
    if(val) f|= (1 << 6);
    else f &= ~(1 << 6);
}

void CPU::set_flag_h(bool val){
    if(val) f|= (1 << 5);
    else f &= ~(1 << 5);
}

void CPU::set_flag_c(bool val){
    if(val) f|= (1 << 4);
    else f &= ~(1 << 4);
}

bool CPU::get_flag_z() const {return (f & (1 << 7)) != 0;}
bool CPU::get_flag_n() const {return (f & (1 << 6)) != 0;}
bool CPU::get_flag_h() const {return (f & (1 << 5)) != 0;}
bool CPU::get_flag_c() const {return (f & (1 << 4)) != 0;}

// Fetch

uint8_t CPU::fetch(){
    uint8_t data = read_byte(pc);
    pc++;
    return data;
}

uint16_t CPU::fetch_16(){
    uint8_t low = fetch();
    uint8_t high = fetch();

    return (high << 8) | low;
}

// Instrucciones...

void CPU::op_nop(){}

// BLOQUE LD
void CPU::op_ld_r8_imm8(uint8_t& reg){reg = fetch();} // LD X, n8.
void CPU::op_ld_r16_imm16(uint8_t& reg_high, uint8_t& reg_low){ reg_low = fetch(); reg_high = fetch();} // LD XX, n16.
void CPU::op_ld_r8_r8(uint8_t& reg1, uint8_t& reg2){reg1 = reg2;} // LD X, X.
void CPU::op_ld_r16mem_r8(uint16_t address, uint8_t& reg){ write_byte(address, reg);} // LD [XX], X.
void CPU::op_ld_r8_r16mem(uint8_t& reg, uint16_t address){ reg = read_byte(address);} // LD X, [XX].

void CPU::op_ld_imm16mem_sp(){ // LD [n16], SP
    uint16_t addr = fetch_16();

    write_byte(addr, sp & 0xFF);
    write_byte(addr + 1, sp >> 8);
} 

void CPU::op_ld_hl_inc_a(){ write_byte(get_hl(),a); set_hl(get_hl() + 1);}
void CPU::op_ld_a_hl_inc(){a = read_byte(get_hl()); set_hl(get_hl() + 1);}

// BLOQUE INC y DEC
void CPU::op_inc_r8(uint8_t& reg){
    bool half_carry = (reg & 0x0F) == 0x0F;

    reg++;
    set_flag_z(reg == 0);
    set_flag_n(false);
    set_flag_h(half_carry);
}
void CPU::op_dec_r8(uint8_t& reg){
    bool half_carry = (reg & 0x0F) == 0x00;
    reg--;

    set_flag_z(reg == 0);
    set_flag_n(true);
    set_flag_h(half_carry);
}
