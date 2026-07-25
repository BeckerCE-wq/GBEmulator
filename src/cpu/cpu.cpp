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

