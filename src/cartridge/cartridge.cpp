#include "cartridge.h"
#include <fstream>
#include <iostream>

bool Cartridge::load_rom(const std::string& filepath){
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if(!file.is_open()){
        std::cerr << "Error: No se pudo abrir el archivo ROM: " << filepath << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();

    file.seekg(0, std::ios::beg);

    rom_data.resize(size);

    if (file.read(reinterpret_cast<char*>(rom_data.data()), size)) {
        std::cout << "ROM cargada con éxito. Tamaño: " << size << " bytes." << std::endl;
        return true;
    }

    return false;
}

uint8_t Cartridge::read_byte(uint16_t address) const{
    if (address < rom_data.size())
    {
        return rom_data[address];
    } else return 0xFF;
}

void Cartridge::write_byte(uint16_t address, uint8_t value){}
/* La memoria del cartucho es de solo lectura en la Game Boy
   Luego en la Game Boy Color tengo entendido que se puede escribir sobre algunos registros
   para los modos de doble velocidad y no sé qué cosa.
   Además que se inventan unos chips llamados MBC1, MBC3 y MBC5. Vienen el mismo cartucho y hacen otras cosas.*/
