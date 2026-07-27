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
    halted = false;
    stopped = false;
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

//----------------------------------------BLOQUE LD----------------------------------------
void CPU::op_ld_r8_imm8(uint8_t& reg){reg = fetch();} // LD X, n8.
void CPU::op_ld_r8_r16mem(uint8_t& reg, uint16_t address){ reg = read_byte(address);} // LD X, [XX].
void CPU::op_ld_r8_r8(uint8_t& reg1, uint8_t& reg2){reg1 = reg2;} // LD X, X.

void CPU::op_ld_r16_imm16(uint8_t& reg_high, uint8_t& reg_low){ reg_low = fetch(); reg_high = fetch();} // LD XX, n16.
void CPU::op_ld_r16mem_r8(uint16_t address, uint8_t& reg){ write_byte(address, reg);} // LD [XX], X.
void CPU::op_ld_r16mem_imm8(uint16_t address){ uint8_t value = fetch(); write_byte(address, value);} // LD [XX], n8

void CPU::op_ld_imm16mem_sp(){ // LD [n16], SP
    uint16_t addr = fetch_16();

    write_byte(addr, sp & 0xFF);
    write_byte(addr + 1, sp >> 8);
}

void CPU::op_ld_hl_inc_a(){ write_byte(get_hl(),a); set_hl(get_hl() + 1);} // LD [HL+], a
void CPU::op_ld_a_hl_inc(){a = read_byte(get_hl()); set_hl(get_hl() + 1);} // LD a, [HL+]

void CPU::op_ld_hl_dec_a(){ write_byte(get_hl(),a); set_hl(get_hl() - 1);} // LD [HL-], a
void CPU::op_ld_a_hl_dec(){a = read_byte(get_hl()); set_hl(get_hl() - 1);} // LD a, [HL-]

void CPU::op_ldh_n16_amem(uint8_t value){write_byte(value + 0xFF00, a);} // LDH [XX], A

void CPU::op_ldh_amem_n16(uint8_t value){ a = read_byte(value + 0xFF00);} // LDH A, [XX]

void CPU::op_ld_hl_sp_e8(){
    uint8_t value = fetch();
    int8_t offset = static_cast<int8_t>(value);

    bool carry = (sp & 0xFF) + (value & 0xFF) > 0xFF;
    bool half_carry = (sp & 0x0F) + (value & 0x0F) > 0x0F;

    set_hl(sp + offset);

    set_flag_z(false);
    set_flag_n(false);
    set_flag_h(half_carry);
    set_flag_c(carry);
}

void CPU::op_ld_sp_hl(){ sp = get_hl();}
// ----------------------------------------BLOQUE INC y DEC----------------------------------------
void CPU::op_inc_r8(uint8_t& reg){ // INC r8
    bool half_carry = (reg & 0x0F) == 0x0F;

    reg++;
    set_flag_z(reg == 0);
    set_flag_n(false);
    set_flag_h(half_carry);
}

void CPU::op_dec_r8(uint8_t& reg){ // DEC r8
    bool half_carry = (reg & 0x0F) == 0x00;
    reg--;

    set_flag_z(reg == 0);
    set_flag_n(true);
    set_flag_h(half_carry);
}

void CPU::op_inc_r16(uint8_t& reg_high, uint8_t& reg_low){ // INC XX. NOTA: PARA SP USAR SP++ EN EL SWITCH, NO ESTE MÉTODO.
    uint16_t val = (reg_high << 8) | reg_low;
    val++;
    reg_high = val >> 8;
    reg_low = val & 0xFF;
}

void CPU::op_dec_r16(uint8_t& reg_high, uint8_t& reg_low){ // DEC XX. TAMBIEN USAR SP--
    uint16_t val = (reg_high << 8) | reg_low;
    val--;
    reg_high = val >> 8;
    reg_low = val & 0xFF;
}

void CPU::op_inc_r16mem(uint16_t address){
    uint8_t val = read_byte(address);
    bool half_carry = (val & 0x0F) == 0x0F;

    val++;
    write_byte(address, val);

    set_flag_z(val == 0);
    set_flag_n(false);
    set_flag_h(half_carry);
}

void CPU::op_dec_r16mem(uint16_t address){
    uint8_t val = read_byte(address);
    bool half_carry = (val & 0x0F) == 0x00;

    val--;
    write_byte(address, val);

    set_flag_z(val == 0);
    set_flag_n(true);
    set_flag_h(half_carry);
}

// ----------------------------------------BLOQUE LÓGICO----------------------------------------

void CPU::op_or_r8(uint8_t reg){
    a |= reg;

    set_flag_z(a == 0);
    set_flag_n(false);
    set_flag_h(false);
    set_flag_c(false);
}

void CPU::op_and_r8(uint8_t reg){
    a &= reg;

    set_flag_z(a == 0);
    set_flag_n(false);
    set_flag_h(true);
    set_flag_c(false);
}

void CPU::op_xor_r8(uint8_t reg){
    a ^= reg;

    set_flag_z(a == 0);
    set_flag_n(false);
    set_flag_h(false);
    set_flag_c(false);
}

// ----------------------------------------BLOQUE ARITMETICO----------------------------------------

void CPU::op_add_r8(uint8_t reg){ // ADD A, X
    int result = a + reg;
    
    bool half_carry = ((a & 0x0F) + (reg & 0x0F)) > 0x0F;
    bool carry = result > 0xFF;
    
    a = static_cast<uint8_t>(result);

    set_flag_z(a == 0);
    set_flag_n(false);
    set_flag_h(half_carry);
    set_flag_c(carry);
}

void CPU::op_sub_r8(uint8_t reg){ // SUB A, X
    int result = a - reg;

    bool carry = reg > a;
    bool half_carry = (reg & 0x0F) > (a & 0x0F);

    a = static_cast<uint8_t>(result);

    set_flag_z(a == 0);
    set_flag_n(true);
    set_flag_h(half_carry);
    set_flag_c(carry);
}

void CPU::op_addc_r8(uint8_t reg){ // ADC A, X
    uint8_t carry_in = get_flag_c() ? 1 : 0;

    int result = a + reg + carry_in;
    
    bool half_carry = ((a & 0x0F) + (reg & 0x0F) + carry_in) > 0x0F;
    bool carry = result > 0xFF;
    
    a = static_cast<uint8_t>(result);

    set_flag_z(a == 0);
    set_flag_n(false);
    set_flag_h(half_carry);
    set_flag_c(carry);
}

void CPU::op_sbc_r8(uint8_t reg){
    uint8_t carry_in = get_flag_c() ? 1 : 0;

    int result = a - reg - carry_in;

    bool half_carry = (static_cast<int>(a & 0x0F) - static_cast<int>(reg & 0x0F) - carry_in) < 0;

    bool carry = result < 0;

    a = static_cast<uint8_t>(result);

    set_flag_z(a == 0);
    set_flag_n(true);
    set_flag_h(half_carry);
    set_flag_c(carry);
}

void CPU::op_cp_r8(uint8_t reg){
    uint8_t result = a - reg;

    bool carry = reg > a;
    bool half_carry = (reg & 0x0F) > (a & 0x0F);

    set_flag_z(result == 0);
    set_flag_n(true);
    set_flag_h(half_carry);
    set_flag_c(carry);
}

void CPU::op_add_r16(uint16_t reg) {
    uint16_t hl_val = get_hl();

    int result = hl_val + reg;

    bool half_carry = ((hl_val & 0x0FFF) + (reg & 0x0FFF)) > 0x0FFF;

    bool carry = result > 0xFFFF;

    set_hl(static_cast<uint16_t>(result));

    set_flag_n(false);
    set_flag_h(half_carry);
    set_flag_c(carry);
}

void CPU::op_add_sp_e8() {
    uint8_t value = fetch();
    int8_t offset = static_cast<int8_t>(value);

    bool half_carry = ((sp & 0x0F) + (value & 0x0F)) > 0x0F;
    bool carry = ((sp & 0xFF) + (value & 0xFF)) > 0xFF;

    sp += offset;

    set_flag_z(false);
    set_flag_n(false);
    set_flag_h(half_carry);
    set_flag_c(carry);
}
// ----------------------------------------BLOQUE POP/PUSH----------------------------------------

void CPU::op_pop_r16(uint8_t& reg_high, uint8_t& reg_low){
    op_ld_r8_r16mem(reg_low, sp);
    sp++;
    op_ld_r8_r16mem(reg_high, sp);
    sp++;

    if (&reg_low == &f)
    {
        f &= 0xF0;
    }
}

void CPU::op_push_r16(uint8_t& reg_high, uint8_t& reg_low) {
    uint8_t low_val = (&reg_low == &f) ? (reg_low & 0xF0) : reg_low;

    sp--;
    op_ld_r16mem_r8(sp, reg_high);

    sp--;
    op_ld_r16mem_r8(sp, low_val);
}

// ----------------------------------------YA NO SÉ QUÉ ES ESTO----------------------------------------
void CPU::op_halt(){ halted = true;}

void CPU::op_stop() {fetch(); stopped = true;}

// ----------------------------------------     ROTACIONES      ----------------------------------------
void CPU::op_rlca() {
    bool bit7 = (a & 0x80) != 0;
    
    a = (a << 1) | (bit7 ? 1 : 0);

    set_flag_z(false);
    set_flag_n(false);
    set_flag_h(false);
    set_flag_c(bit7);
}

void CPU::op_rla() {
    bool bit7 = (a & 0x80) != 0;
    bool old_carry = get_flag_c();

    a = (a << 1) | (old_carry ? 1 : 0);

    set_flag_z(false);
    set_flag_n(false);
    set_flag_h(false);
    set_flag_c(bit7);
}

void CPU::op_rrca() {
    bool bit0 = (a & 0x01) != 0;
    
    a = (a >> 1) | (bit0 ? 0x80 : 0);

    set_flag_z(false);
    set_flag_n(false);
    set_flag_h(false);
    set_flag_c(bit0);
}

void CPU::op_rra(){
    bool bit0 = (a & 0x01) != 0;
    bool old_carry = get_flag_c();

    a = (a >> 1) | (old_carry ? 0x80 : 0);

    set_flag_z(false);
    set_flag_n(false);
    set_flag_h(false);
    set_flag_c(bit0);
}

// ----------------------------------------     SALTOS         ----------------------------------------
void CPU::op_jr_condition_e8(bool condition){
    int8_t offset = static_cast<int8_t>(fetch());

    if (condition) pc += offset;
}

void CPU::op_jp_condition_n16(bool condition){
    uint16_t addr = fetch_16();

    if (condition) pc = addr;
}

void CPU::op_jp_hl(){ pc = get_hl();}

void CPU::op_call_condition_n16(bool condition){
    uint16_t address = fetch_16();

    if (condition)
    {
        sp--;
        write_byte(sp, static_cast<uint8_t>(pc >> 8));

        sp--;
        write_byte(sp, static_cast<uint8_t>(pc & 0xFF));

        pc = address;
    }
}

void CPU::op_ret_condition(bool condition){
    if (condition)
    {
        uint8_t low = read_byte(sp);
        sp++;

        uint8_t high = read_byte(sp);
        sp++;

        pc = (static_cast<uint16_t>(high) << 8) | low;
    }
}

void CPU::op_reti(){
    op_ret_condition(true);
    ime = true;
}
