#include "Vtarunes_top.h"
#include "Vtarunes_top___024root.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr uint8_t CPU_STATE_DECODE = 5;

enum class AddrMode {
    Implied,
    Imm,
    Zp,
    Zpx,
    Zpy,
    Abs,
    Absx,
    Absy,
    Ind,
    Indx,
    Indy,
    Rel,
};

struct OpcodeInfo {
    const char* mnemonic;
    AddrMode mode;
    uint8_t len;
};

std::string hex8(uint8_t value) {
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
        << static_cast<int>(value);
    return oss.str();
}

std::string hex16(uint16_t value) {
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(4)
        << static_cast<int>(value);
    return oss.str();
}

OpcodeInfo decodeOpcode(uint8_t opcode) {
    switch (opcode) {
        case 0x00: return {"BRK", AddrMode::Implied, 2};
        case 0x40: return {"RTI", AddrMode::Implied, 1};
        case 0x48: return {"PHA", AddrMode::Implied, 1};
        case 0x68: return {"PLA", AddrMode::Implied, 1};
        case 0x08: return {"PHP", AddrMode::Implied, 1};
        case 0x28: return {"PLP", AddrMode::Implied, 1};
        case 0x78: return {"SEI", AddrMode::Implied, 1};
        case 0x38: return {"SEC", AddrMode::Implied, 1};
        case 0x58: return {"CLI", AddrMode::Implied, 1};
        case 0xD8: return {"CLD", AddrMode::Implied, 1};
        case 0x18: return {"CLC", AddrMode::Implied, 1};
        case 0xF8: return {"SED", AddrMode::Implied, 1};
        case 0xB8: return {"CLV", AddrMode::Implied, 1};
        case 0xEA: return {"NOP", AddrMode::Implied, 1};
        case 0x9A: return {"TXS", AddrMode::Implied, 1};
        case 0xBA: return {"TSX", AddrMode::Implied, 1};
        case 0xAA: return {"TAX", AddrMode::Implied, 1};
        case 0x8A: return {"TXA", AddrMode::Implied, 1};
        case 0xA8: return {"TAY", AddrMode::Implied, 1};
        case 0x98: return {"TYA", AddrMode::Implied, 1};
        case 0xE8: return {"INX", AddrMode::Implied, 1};
        case 0xC8: return {"INY", AddrMode::Implied, 1};
        case 0xCA: return {"DEX", AddrMode::Implied, 1};
        case 0x88: return {"DEY", AddrMode::Implied, 1};

        case 0xE6: return {"INC", AddrMode::Zp, 2};
        case 0xF6: return {"INC", AddrMode::Zpx, 2};
        case 0xEE: return {"INC", AddrMode::Abs, 3};
        case 0xFE: return {"INC", AddrMode::Absx, 3};
        case 0xC6: return {"DEC", AddrMode::Zp, 2};
        case 0xD6: return {"DEC", AddrMode::Zpx, 2};
        case 0xCE: return {"DEC", AddrMode::Abs, 3};
        case 0xDE: return {"DEC", AddrMode::Absx, 3};
        case 0x0A: return {"ASL", AddrMode::Implied, 1};
        case 0x06: return {"ASL", AddrMode::Zp, 2};
        case 0x16: return {"ASL", AddrMode::Zpx, 2};
        case 0x0E: return {"ASL", AddrMode::Abs, 3};
        case 0x1E: return {"ASL", AddrMode::Absx, 3};
        case 0x4A: return {"LSR", AddrMode::Implied, 1};
        case 0x46: return {"LSR", AddrMode::Zp, 2};
        case 0x56: return {"LSR", AddrMode::Zpx, 2};
        case 0x4E: return {"LSR", AddrMode::Abs, 3};
        case 0x5E: return {"LSR", AddrMode::Absx, 3};
        case 0x2A: return {"ROL", AddrMode::Implied, 1};
        case 0x26: return {"ROL", AddrMode::Zp, 2};
        case 0x36: return {"ROL", AddrMode::Zpx, 2};
        case 0x2E: return {"ROL", AddrMode::Abs, 3};
        case 0x3E: return {"ROL", AddrMode::Absx, 3};
        case 0x6A: return {"ROR", AddrMode::Implied, 1};
        case 0x66: return {"ROR", AddrMode::Zp, 2};
        case 0x76: return {"ROR", AddrMode::Zpx, 2};
        case 0x6E: return {"ROR", AddrMode::Abs, 3};
        case 0x7E: return {"ROR", AddrMode::Absx, 3};

        case 0xA9: return {"LDA", AddrMode::Imm, 2};
        case 0xA5: return {"LDA", AddrMode::Zp, 2};
        case 0xB5: return {"LDA", AddrMode::Zpx, 2};
        case 0xAD: return {"LDA", AddrMode::Abs, 3};
        case 0xBD: return {"LDA", AddrMode::Absx, 3};
        case 0xB9: return {"LDA", AddrMode::Absy, 3};
        case 0xA1: return {"LDA", AddrMode::Indx, 2};
        case 0xB1: return {"LDA", AddrMode::Indy, 2};
        case 0xA2: return {"LDX", AddrMode::Imm, 2};
        case 0xA6: return {"LDX", AddrMode::Zp, 2};
        case 0xB6: return {"LDX", AddrMode::Zpy, 2};
        case 0xAE: return {"LDX", AddrMode::Abs, 3};
        case 0xBE: return {"LDX", AddrMode::Absy, 3};
        case 0xA0: return {"LDY", AddrMode::Imm, 2};
        case 0xA4: return {"LDY", AddrMode::Zp, 2};
        case 0xB4: return {"LDY", AddrMode::Zpx, 2};
        case 0xAC: return {"LDY", AddrMode::Abs, 3};
        case 0xBC: return {"LDY", AddrMode::Absx, 3};

        case 0xE0: return {"CPX", AddrMode::Imm, 2};
        case 0xE4: return {"CPX", AddrMode::Zp, 2};
        case 0xEC: return {"CPX", AddrMode::Abs, 3};
        case 0xC0: return {"CPY", AddrMode::Imm, 2};
        case 0xC4: return {"CPY", AddrMode::Zp, 2};
        case 0xCC: return {"CPY", AddrMode::Abs, 3};
        case 0xC9: return {"CMP", AddrMode::Imm, 2};
        case 0xC5: return {"CMP", AddrMode::Zp, 2};
        case 0xD5: return {"CMP", AddrMode::Zpx, 2};
        case 0xCD: return {"CMP", AddrMode::Abs, 3};
        case 0xDD: return {"CMP", AddrMode::Absx, 3};
        case 0xD9: return {"CMP", AddrMode::Absy, 3};
        case 0xC1: return {"CMP", AddrMode::Indx, 2};
        case 0xD1: return {"CMP", AddrMode::Indy, 2};

        case 0x69: return {"ADC", AddrMode::Imm, 2};
        case 0x65: return {"ADC", AddrMode::Zp, 2};
        case 0x75: return {"ADC", AddrMode::Zpx, 2};
        case 0x6D: return {"ADC", AddrMode::Abs, 3};
        case 0x7D: return {"ADC", AddrMode::Absx, 3};
        case 0x79: return {"ADC", AddrMode::Absy, 3};
        case 0x61: return {"ADC", AddrMode::Indx, 2};
        case 0x71: return {"ADC", AddrMode::Indy, 2};
        case 0xE9: return {"SBC", AddrMode::Imm, 2};
        case 0xE5: return {"SBC", AddrMode::Zp, 2};
        case 0xF5: return {"SBC", AddrMode::Zpx, 2};
        case 0xED: return {"SBC", AddrMode::Abs, 3};
        case 0xFD: return {"SBC", AddrMode::Absx, 3};
        case 0xF9: return {"SBC", AddrMode::Absy, 3};
        case 0xE1: return {"SBC", AddrMode::Indx, 2};
        case 0xF1: return {"SBC", AddrMode::Indy, 2};
        case 0x09: return {"ORA", AddrMode::Imm, 2};
        case 0x05: return {"ORA", AddrMode::Zp, 2};
        case 0x15: return {"ORA", AddrMode::Zpx, 2};
        case 0x0D: return {"ORA", AddrMode::Abs, 3};
        case 0x1D: return {"ORA", AddrMode::Absx, 3};
        case 0x19: return {"ORA", AddrMode::Absy, 3};
        case 0x01: return {"ORA", AddrMode::Indx, 2};
        case 0x11: return {"ORA", AddrMode::Indy, 2};
        case 0x29: return {"AND", AddrMode::Imm, 2};
        case 0x25: return {"AND", AddrMode::Zp, 2};
        case 0x35: return {"AND", AddrMode::Zpx, 2};
        case 0x2D: return {"AND", AddrMode::Abs, 3};
        case 0x3D: return {"AND", AddrMode::Absx, 3};
        case 0x39: return {"AND", AddrMode::Absy, 3};
        case 0x21: return {"AND", AddrMode::Indx, 2};
        case 0x31: return {"AND", AddrMode::Indy, 2};
        case 0x49: return {"EOR", AddrMode::Imm, 2};
        case 0x45: return {"EOR", AddrMode::Zp, 2};
        case 0x55: return {"EOR", AddrMode::Zpx, 2};
        case 0x4D: return {"EOR", AddrMode::Abs, 3};
        case 0x5D: return {"EOR", AddrMode::Absx, 3};
        case 0x59: return {"EOR", AddrMode::Absy, 3};
        case 0x41: return {"EOR", AddrMode::Indx, 2};
        case 0x51: return {"EOR", AddrMode::Indy, 2};
        case 0x24: return {"BIT", AddrMode::Zp, 2};
        case 0x2C: return {"BIT", AddrMode::Abs, 3};

        case 0x85: return {"STA", AddrMode::Zp, 2};
        case 0x95: return {"STA", AddrMode::Zpx, 2};
        case 0x8D: return {"STA", AddrMode::Abs, 3};
        case 0x9D: return {"STA", AddrMode::Absx, 3};
        case 0x99: return {"STA", AddrMode::Absy, 3};
        case 0x81: return {"STA", AddrMode::Indx, 2};
        case 0x91: return {"STA", AddrMode::Indy, 2};
        case 0x86: return {"STX", AddrMode::Zp, 2};
        case 0x96: return {"STX", AddrMode::Zpy, 2};
        case 0x8E: return {"STX", AddrMode::Abs, 3};
        case 0x84: return {"STY", AddrMode::Zp, 2};
        case 0x94: return {"STY", AddrMode::Zpx, 2};
        case 0x8C: return {"STY", AddrMode::Abs, 3};

        case 0x90: return {"BCC", AddrMode::Rel, 2};
        case 0xB0: return {"BCS", AddrMode::Rel, 2};
        case 0xD0: return {"BNE", AddrMode::Rel, 2};
        case 0xF0: return {"BEQ", AddrMode::Rel, 2};
        case 0x10: return {"BPL", AddrMode::Rel, 2};
        case 0x30: return {"BMI", AddrMode::Rel, 2};
        case 0x50: return {"BVC", AddrMode::Rel, 2};
        case 0x70: return {"BVS", AddrMode::Rel, 2};
        case 0x4C: return {"JMP", AddrMode::Abs, 3};
        case 0x6C: return {"JMP", AddrMode::Ind, 3};
        case 0x20: return {"JSR", AddrMode::Abs, 3};
        case 0x60: return {"RTS", AddrMode::Implied, 1};
        default: return {"???", AddrMode::Implied, 1};
    }
}

const char* mmioName(uint16_t addr) {
    switch (addr) {
        case 0x2000: return "PpuCtrl_2000";
        case 0x2001: return "PpuMask_2001";
        case 0x2002: return "PpuStatus_2002";
        case 0x2005: return "PpuScroll_2005";
        case 0x2006: return "PpuAddr_2006";
        case 0x2007: return "PpuData_2007";
        case 0x4016: return "Joypad1_4016";
        case 0x4017: return "Joypad2_4017";
        default: return nullptr;
    }
}

bool isReadInst(const char* mnemonic) {
    return std::strcmp(mnemonic, "LDA") == 0 || std::strcmp(mnemonic, "LDX") == 0
        || std::strcmp(mnemonic, "LDY") == 0 || std::strcmp(mnemonic, "BIT") == 0
        || std::strcmp(mnemonic, "CMP") == 0 || std::strcmp(mnemonic, "CPX") == 0
        || std::strcmp(mnemonic, "CPY") == 0 || std::strcmp(mnemonic, "ADC") == 0
        || std::strcmp(mnemonic, "SBC") == 0 || std::strcmp(mnemonic, "ORA") == 0
        || std::strcmp(mnemonic, "AND") == 0 || std::strcmp(mnemonic, "EOR") == 0;
}

class CpuTracer {
public:
    void trace(Vtarunes_top* dut, int frameCount, uint64_t cpuCycle) {
        auto* root = dut->rootp;
        const uint8_t state = root->tarunes_top__DOT__cpu_inst__DOT__state;
        const bool enteringDecode = state == CPU_STATE_DECODE && prevState_ != CPU_STATE_DECODE;
        prevState_ = state;
        if (!enteringDecode) {
            return;
        }

        const uint16_t pc = root->tarunes_top__DOT__cpu_inst__DOT__reg_pc;
        const uint8_t opcode = readByte(root, pc);
        const uint8_t op1 = readByte(root, pc + 1);
        const uint8_t op2 = readByte(root, pc + 2);
        const OpcodeInfo info = decodeOpcode(opcode);
        const std::string disasm = formatDisasm(root, pc, info, op1, op2);
        const std::string bytes = formatBytes(info.len, opcode, op1, op2);
        const std::string flags = formatFlags(root->tarunes_top__DOT__cpu_inst__DOT__reg_p);

        std::printf(
            "%04X    %-30s A:%02X X:%02X Y:%02X S:%02X P:%s V:%-3d H:%-3d Fr:%-3d Cycle:%llu BC:%s\n",
            pc,
            disasm.c_str(),
            root->tarunes_top__DOT__cpu_inst__DOT__reg_a,
            root->tarunes_top__DOT__cpu_inst__DOT__reg_x,
            root->tarunes_top__DOT__cpu_inst__DOT__reg_y,
            root->tarunes_top__DOT__cpu_inst__DOT__reg_sp,
            flags.c_str(),
            static_cast<int>(dut->scanline),
            static_cast<int>(dut->cycle),
            frameCount,
            static_cast<unsigned long long>(cpuCycle),
            bytes.c_str());
    }

private:
    uint8_t prevState_ = 0xff;

    uint8_t readByte(Vtarunes_top___024root* root, uint16_t addr) const {
        if (addr < 0x2000) {
            return root->tarunes_top__DOT__wram__DOT__mem[addr & 0x07ff];
        }
        if (addr >= 0x8000) {
            return root->tarunes_top__DOT__prom__DOT__mem[addr & 0x7fff];
        }
        return 0;
    }

    bool readDataByte(Vtarunes_top___024root* root, uint16_t addr, uint8_t* value) const {
        if (addr < 0x2000) {
            *value = root->tarunes_top__DOT__wram__DOT__mem[addr & 0x07ff];
            return true;
        }
        if (addr >= 0x8000) {
            *value = root->tarunes_top__DOT__prom__DOT__mem[addr & 0x7fff];
            return true;
        }
        if (addr == 0x2002) {
            *value = (root->scanline >= 241 && root->scanline <= 260) ? 0x80 : 0x00;
            return true;
        }
        return false;
    }

    std::string formatFlags(uint8_t p) const {
        std::string out = "nv--dizc";
        if (p & 0x80) out[0] = 'N';
        if (p & 0x40) out[1] = 'V';
        if (p & 0x08) out[4] = 'D';
        if (p & 0x04) out[5] = 'I';
        if (p & 0x02) out[6] = 'Z';
        if (p & 0x01) out[7] = 'C';
        return out;
    }

    std::string formatBytes(uint8_t len, uint8_t opcode, uint8_t op1, uint8_t op2) const {
        std::string out = hex8(opcode);
        if (len >= 2) out += " " + hex8(op1);
        if (len >= 3) out += " " + hex8(op2);
        return out;
    }

    std::string absOperand(const char* mnemonic, uint16_t addr) const {
        const char* name = mmioName(addr);
        if (name != nullptr) {
            return std::string(mnemonic) + " " + name;
        }
        return std::string(mnemonic) + " $" + hex16(addr);
    }

    std::string formatDisasm(
        Vtarunes_top___024root* root,
        uint16_t pc,
        const OpcodeInfo& info,
        uint8_t op1,
        uint8_t op2) const {
        const uint16_t addr = static_cast<uint16_t>(op1) | (static_cast<uint16_t>(op2) << 8);
        std::string text;

        switch (info.mode) {
            case AddrMode::Implied:
                text = info.mnemonic;
                break;
            case AddrMode::Imm:
                text = std::string(info.mnemonic) + " #$" + hex8(op1);
                break;
            case AddrMode::Zp:
                text = std::string(info.mnemonic) + " $" + hex8(op1);
                break;
            case AddrMode::Zpx:
                text = std::string(info.mnemonic) + " $" + hex8(op1) + ",X";
                break;
            case AddrMode::Zpy:
                text = std::string(info.mnemonic) + " $" + hex8(op1) + ",Y";
                break;
            case AddrMode::Abs:
                text = absOperand(info.mnemonic, addr);
                break;
            case AddrMode::Absx:
                text = absOperand(info.mnemonic, addr) + ",X";
                break;
            case AddrMode::Absy:
                text = absOperand(info.mnemonic, addr) + ",Y";
                break;
            case AddrMode::Ind:
                text = std::string(info.mnemonic) + " ($" + hex16(addr) + ")";
                break;
            case AddrMode::Indx:
                text = std::string(info.mnemonic) + " ($" + hex8(op1) + ",X)";
                break;
            case AddrMode::Indy:
                text = std::string(info.mnemonic) + " ($" + hex8(op1) + "),Y";
                break;
            case AddrMode::Rel: {
                const uint16_t target = static_cast<uint16_t>(pc + 2 + static_cast<int8_t>(op1));
                text = std::string(info.mnemonic) + " $" + hex16(target);
                break;
            }
        }

        uint8_t value = 0;
        if (isReadInst(info.mnemonic)) {
            if (info.mode == AddrMode::Abs && readDataByte(root, addr, &value)) {
                text += " = $" + hex8(value);
            } else if (info.mode == AddrMode::Absx) {
                const uint16_t effective =
                    static_cast<uint16_t>(addr + root->tarunes_top__DOT__cpu_inst__DOT__reg_x);
                if (readDataByte(root, effective, &value)) {
                    text += " @ $" + hex16(effective) + " = $" + hex8(value);
                }
            } else if (info.mode == AddrMode::Absy) {
                const uint16_t effective =
                    static_cast<uint16_t>(addr + root->tarunes_top__DOT__cpu_inst__DOT__reg_y);
                if (readDataByte(root, effective, &value)) {
                    text += " @ $" + hex16(effective) + " = $" + hex8(value);
                }
            } else if (info.mode == AddrMode::Zp && readDataByte(root, op1, &value)) {
                text += " = $" + hex8(value);
            } else if (info.mode == AddrMode::Zpx) {
                const uint8_t effective =
                    static_cast<uint8_t>(op1 + root->tarunes_top__DOT__cpu_inst__DOT__reg_x);
                if (readDataByte(root, effective, &value)) {
                    text += " @ $" + hex8(effective) + " = $" + hex8(value);
                }
            } else if (info.mode == AddrMode::Zpy) {
                const uint8_t effective =
                    static_cast<uint8_t>(op1 + root->tarunes_top__DOT__cpu_inst__DOT__reg_y);
                if (readDataByte(root, effective, &value)) {
                    text += " @ $" + hex8(effective) + " = $" + hex8(value);
                }
            }
        }

        return text;
    }
};

bool saveFrameBmp(const char* path, void* pixels, int width, int height, int pitch) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
        pixels,
        width,
        height,
        32,
        pitch,
        SDL_PIXELFORMAT_ARGB8888);
    if (surface == nullptr) {
        std::fprintf(stderr, "SDL_CreateRGBSurfaceWithFormatFrom Error: %s\n", SDL_GetError());
        return false;
    }

    const int result = SDL_SaveBMP(surface, path);
    SDL_FreeSurface(surface);
    if (result != 0) {
        std::fprintf(stderr, "SDL_SaveBMP Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    Verilated::traceEverOn(true);

    Vtarunes_top* dut = new Vtarunes_top;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("wave.vcd");

    // SDL2の初期化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    // ウィンドウサイズ: 256x240を2倍スケーリング -> 512x480
    const int SCREEN_WIDTH = 256;
    const int SCREEN_HEIGHT = 240;
    const int SCALE = 2;
    const int WINDOW_WIDTH = SCREEN_WIDTH * SCALE;
    const int WINDOW_HEIGHT = SCREEN_HEIGHT * SCALE;

    SDL_Window* window = SDL_CreateWindow("tarunes",
                                          100,
                                          100,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!texture) {
        fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    SDL_AudioSpec desired{};
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 1;
    desired.samples = 1024;

    SDL_AudioSpec obtained{};
    SDL_AudioDeviceID audio_dev = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    if (audio_dev == 0) {
        fprintf(stderr, "SDL_OpenAudioDevice Error: %s\n", SDL_GetError());
    } else {
        SDL_PauseAudioDevice(audio_dev, 1);
    }

    // ピクセルバッファ (ARGB8888)
    uint32_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    uint32_t last_frame_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    memset(pixel_buffer, 0, sizeof(pixel_buffer));
    memset(last_frame_buffer, 0, sizeof(last_frame_buffer));

    dut->clk = 0;
    dut->rst = 0;
    dut->controller1_btns = 0;
    dut->controller2_btns = 0;

    const int RESET_CYCLES = 4;
    int frame_count = 0;
    int prev_scanline = 0;
    bool running = true;
    bool has_completed_frame = false;
    uint8_t controller1_btns = 0;
    const double CPU_CLOCK_HZ = 1789773.0;
    const double AUDIO_SAMPLE_RATE = static_cast<double>(obtained.freq ? obtained.freq : desired.freq);
    double audio_phase = 0.0;
    double audio_stretch = 8.0;
    double audio_stretch_phase = 0.0;
    uint64_t audio_cycle_count = 0;
    uint64_t speed_sample_cycle = 0;
    uint64_t speed_sample_ticks = SDL_GetPerformanceCounter();
    const uint64_t PERF_FREQ = SDL_GetPerformanceFrequency();
    const uint64_t SPEED_SAMPLE_CYCLES = 8192;
    const uint32_t AUDIO_QUEUE_LOW_BYTES = 8192;
    const uint32_t AUDIO_QUEUE_HIGH_BYTES = 32768;
    const uint32_t AUDIO_QUEUE_MAX_BYTES = 65536;
    bool audio_paused = true;
    std::vector<int16_t> audio_buffer;
    audio_buffer.reserve(4096);
    uint64_t cpu_cycle = 0;
    CpuTracer cpu_tracer;

    // メインループ
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                bool pressed = event.type == SDL_KEYDOWN;
                uint8_t mask = 0;
                switch (event.key.keysym.sym) {
                    case SDLK_z:      mask = 1 << 0; break; // A
                    case SDLK_x:      mask = 1 << 1; break; // B
                    case SDLK_RSHIFT: mask = 1 << 2; break; // Select
                    case SDLK_RETURN: mask = 1 << 3; break; // Start
                    case SDLK_UP:     mask = 1 << 4; break;
                    case SDLK_DOWN:   mask = 1 << 5; break;
                    case SDLK_LEFT:   mask = 1 << 6; break;
                    case SDLK_RIGHT:  mask = 1 << 7; break;
                    case SDLK_ESCAPE: running = false; break;
                    default: break;
                }

                if (mask != 0) {
                    if (pressed) {
                        controller1_btns |= mask;
                    } else {
                        controller1_btns &= ~mask;
                    }
                }
            }
        }
        dut->controller1_btns = controller1_btns;
        dut->controller2_btns = 0;

        // 1サイクルシミュレーション
        for (int half = 0; half < 2; half++) {
            if (Verilated::time() == RESET_CYCLES) {
                dut->rst = 1;
            }

            dut->clk = half;
            dut->eval();
            tfp->dump(Verilated::time());
            Verilated::timeInc(1);
        }
        if (dut->rst) {
            cpu_cycle++;
        }
        cpu_tracer.trace(dut, frame_count, cpu_cycle);

        if (audio_dev != 0) {
            audio_cycle_count++;
            if (audio_cycle_count - speed_sample_cycle >= SPEED_SAMPLE_CYCLES) {
                uint64_t now = SDL_GetPerformanceCounter();
                double wall_seconds = static_cast<double>(now - speed_sample_ticks) / static_cast<double>(PERF_FREQ);
                double emu_seconds = static_cast<double>(audio_cycle_count - speed_sample_cycle) / CPU_CLOCK_HZ;
                if (wall_seconds > 0.0) {
                    audio_stretch = std::clamp(wall_seconds / emu_seconds, 1.0, 64.0);
                }
                speed_sample_cycle = audio_cycle_count;
                speed_sample_ticks = now;
            }

            audio_phase += AUDIO_SAMPLE_RATE;
            while (audio_phase >= CPU_CLOCK_HZ) {
                audio_phase -= CPU_CLOCK_HZ;
                int centered = static_cast<int>(dut->audio_sample) - 128;
                int16_t sample = static_cast<int16_t>(centered * 192);
                audio_stretch_phase += audio_stretch;
                int repeat_count = static_cast<int>(audio_stretch_phase);
                audio_stretch_phase -= repeat_count;
                for (int i = 0; i < repeat_count; i++) {
                    audio_buffer.push_back(sample);
                }
            }

            uint32_t queued_bytes = SDL_GetQueuedAudioSize(audio_dev);
            if (audio_buffer.size() >= 512 && queued_bytes < AUDIO_QUEUE_MAX_BYTES) {
                SDL_QueueAudio(audio_dev, audio_buffer.data(), audio_buffer.size() * sizeof(int16_t));
                audio_buffer.clear();
                queued_bytes = SDL_GetQueuedAudioSize(audio_dev);
            }

            if (audio_paused && queued_bytes >= AUDIO_QUEUE_HIGH_BYTES) {
                SDL_PauseAudioDevice(audio_dev, 0);
                audio_paused = false;
            } else if (!audio_paused && queued_bytes <= AUDIO_QUEUE_LOW_BYTES) {
                SDL_PauseAudioDevice(audio_dev, 1);
                audio_paused = true;
            }
        }

        // 有効ピクセル領域かチェック
        int scanline = dut->scanline;
        int cycle = dut->cycle;
        int pixel_r = dut->pixel_r;
        int pixel_g = dut->pixel_g;
        int pixel_b = dut->pixel_b;

        if (cycle < SCREEN_WIDTH && scanline < SCREEN_HEIGHT) {
            // 直接RGB値を使用
            // ARGB8888形式に変換 (Alpha=0xFF, R, G, B)
            pixel_buffer[scanline][cycle] = 0xFF000000 | 
                                           ((pixel_r & 0xFF) << 16) |
                                           ((pixel_g & 0xFF) << 8) |
                                           (pixel_b & 0xFF);
        }

        // フレームカウント: scanlineが0に戻ったら1フレーム完了
        if (prev_scanline == 261 && scanline == 0) {
            frame_count++;
            printf("Frame %d completed\n", frame_count);
            memcpy(last_frame_buffer, pixel_buffer, sizeof(last_frame_buffer));
            has_completed_frame = true;

            // ピクセルバッファをテクスチャにコピーして表示
            SDL_UpdateTexture(texture, NULL, pixel_buffer, SCREEN_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        prev_scanline = scanline;
    }

    printf("Simulation finished after %d frames\n", frame_count);
    if (saveFrameBmp(
            "last_frame.bmp",
            has_completed_frame ? static_cast<void*>(last_frame_buffer) : static_cast<void*>(pixel_buffer),
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SCREEN_WIDTH * sizeof(uint32_t))) {
        printf("Saved last frame to last_frame.bmp\n");
    }

    // クリーンアップ
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (audio_dev != 0) {
        SDL_CloseAudioDevice(audio_dev);
    }
    SDL_Quit();

    tfp->close();
    delete dut;
    delete tfp;
    return 0;
}
