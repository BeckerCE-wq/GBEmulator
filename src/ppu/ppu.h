#pragma once
#include <cstdint>
#include <array>

enum class PPUMode : uint8_t {
    HBlank = 0,
    VBlank = 1,
    OAMSearch = 2,
    PixelTransfer = 3
};

class PPU{
public:
    PPU();

    void tick(uint8_t m_cycles);

    // Métodos para el Bus
    uint8_t read_vram(uint16_t addr) const;
    void write_vram(uint16_t addr, uint8_t val);

    uint8_t read_oam(uint16_t addr) const;
    void write_oam(uint16_t addr, uint8_t val);

    uint8_t read_register(uint16_t addr) const;
    void write_register(uint16_t addr, uint8_t val);

    const uint32_t* get_frame_buffer() const { return frame_buffer.data(); }

private:

    std::array<uint8_t, 8192> vram{}; // 0x8000 - 0x9FFF
    std::array<uint8_t, 160> oam{};   // 0xFE00 - 0xFE9F

    uint8_t lcdc = 0x91;
    uint8_t stat = 0;
    uint8_t scy = 0;
    uint8_t scx = 0;
    uint8_t ly = 0;
    uint8_t lyc = 0;
    uint8_t bgp = 0xFC;
    uint8_t obp0 = 0xFF;
    uint8_t obp1 = 0xFF;
    uint8_t wy = 0;
    uint8_t wx = 0;

    // 3. Control interno y timing
    uint16_t dots = 0; // Se incrementa hasta 456 T-Cycles por scanline
    PPUMode mode = PPUMode::OAMSearch;
    std::array<uint32_t, 160 * 144> frame_buffer{};

    // Helpers internos de renderizado
    void render_scanline();
};
