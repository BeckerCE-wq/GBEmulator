#pragma once
#include "timer.h"
#include "cstdint"

    // Esta parte me costó un montón de entender, porque tiene que ver con la frecuencia del cuarzo y no sé qué más
    // pero la idea es la siguiente:

    // La Game Boy tiene un cuarzo que vibra a 4,19 MHz. Esta es la velocidad de los T-Cycles.
    // La CPU procesa todo en 1,05 MHz. Es decir, en bloques de 4 T-Cycles, que son los M-Cycles.
    // Acá viene el que comepete en este .cpp, el registro DIV (0xFF04) se incrementa cada a 16.384 Hz
    // Ahora bien, la implementación que estoy haciendo usa un registro de 16 bits en lugar de uno de 8, como sería el DIV original en una GB.
    // El chiste es que este registro tendría que aumentarse cada T-Cycles, es decir, a las chapas.
    // Anda tan rápido que para el CPU recién es razonable poder operar apartir a del bit 8, que sería el comienzo del registro DIV (0xFF04).

    // Entonces, esto sirve para determinar cuando tiene que incrementarse el registro TIMA (0xFF05).
    // Según la combinación de los primeros dos bits de TAC, TIMA se incrementa una frecuencia distinta.
    // Cada una, la podemos sacar de un bit de div_counter.

    // Algo más o menos así entendí del Pan Docs y lo que me explicó Gemini, hay una tabla incluso. Invito a verla,
    // de seguro es mucho mejor explicación de la que pueda dar yo.

bool Timer::get_timer_bit() const{
    if (!(tac & 0x04)) return false; // Si el tercer bit de TAC está en 0, el Timer está desactivado, no hay pulso.

    // Según los bits 1-0 de TAC, elegimos qué bit de div_counter mirar:
    // 00 -> Bit 9  (4096 Hz)
    // 01 -> Bit 3  (262144 Hz)
    // 10 -> Bit 5  (65536 Hz)
    // 11 -> Bit 7  (16384 Hz)

    switch (tac & 0x03) {
        case 0b00: return (div_counter & (1 << 9)) != 0;
        case 0b01: return (div_counter & (1 << 3)) != 0;
        case 0b10: return (div_counter & (1 << 5)) != 0;
        case 0b11: return (div_counter & (1 << 7)) != 0;
    }
    return false;

}

void Timer::tick(uint8_t m_cycles, bool& interrupt_flag) {
    uint16_t t_cycles = 4 * m_cycles;

    // Avanzamos T-Cycle por T-Cycle para no perder ningún flanco de bajada
    for (uint16_t i = 0; i < t_cycles; i++) {
        div_counter++;

        bool current_bit = get_timer_bit();

        if (prev_timer_bit && !current_bit) { // Flanco de bajada
            if (tima == 0xFF) {
                tima = tma;
                interrupt_flag = true;
            } else {
                tima++;
            }
        }

        prev_timer_bit = current_bit;
    }
}

uint8_t Timer::read_byte(uint16_t address) const{
    switch (address){
        case 0xFF04: return static_cast<uint8_t>(div_counter >> 8);
        case 0xFF05: return tima;
        case 0xFF06: return tma;
        case 0xFF07: return tac | 0xF8;

        default: return 0xFF;
    }
}

void Timer::write_byte(uint16_t address, uint8_t value){
    switch (address){
        case 0xFF04: div_counter = 0; break;
        case 0xFF05: tima = value; break;
        case 0xFF06: tma = value; break;
        case 0xFF07: tac = value & 0x07; break;
    }
}
