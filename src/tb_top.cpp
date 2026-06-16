#include "Vtarunes_top.h"
#include "Vtarunes_top___024root.h"
#include "Vtarunes_top_tarunes___05F_bus_if___05F8___05F3.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
constexpr int NES_WIDTH = 256;
constexpr int NES_HEIGHT = 240;
constexpr int HDMI_WIDTH = 720;
constexpr int HDMI_HEIGHT = 480;

constexpr std::array<uint32_t, 64> NES_PALETTE_RGB = {
    0x808080, 0x003DA6, 0x0012B0, 0x440096, 0xA1005E, 0xC70028, 0xBA0600, 0x8C1700,
    0x5C2F00, 0x104500, 0x054A00, 0x00472E, 0x004166, 0x000000, 0x050505, 0x050505,
    0xC7C7C7, 0x0077FF, 0x2155FF, 0x8237FA, 0xEB2FB5, 0xFF2950, 0xFF2200, 0xD63200,
    0xC46200, 0x358000, 0x058F00, 0x008A55, 0x0099CC, 0x212121, 0x090909, 0x090909,
    0xFFFFFF, 0x0FD7FF, 0x69A2FF, 0xD480FF, 0xFF45F3, 0xFF618B, 0xFF8833, 0xFF9C12,
    0xFABC20, 0x9FE30E, 0x2BF035, 0x0CF0A4, 0x05FBFF, 0x5E5E5E, 0x0D0D0D, 0x0D0D0D,
    0xFFFFFF, 0xA6FCFF, 0xB3ECFF, 0xDAABEB, 0xFFA8F9, 0xFFABB3, 0xFFD2B0, 0xFFEFA6,
    0xFFF79C, 0xD7E895, 0xA6EDAF, 0xA2F2DA, 0x99FFFC, 0xDDDDDD, 0x111111, 0x111111,
};

uint32_t argbFromRgb24(uint32_t rgb) {
    return 0xFF000000 | (rgb & 0x00FFFFFF);
}

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
        case 0x2003: return "OamAddr_2003";
        case 0x2004: return "OamData_2004";
        case 0x2005: return "PpuScroll_2005";
        case 0x2006: return "PpuAddr_2006";
        case 0x2007: return "PpuData_2007";
        case 0x4000: return "Pulse1Ctrl_4000";
        case 0x4001: return "Pulse1Sweep_4001";
        case 0x4002: return "Pulse1TimerLo_4002";
        case 0x4003: return "Pulse1TimerHi_4003";
        case 0x4004: return "Pulse2Ctrl_4004";
        case 0x4005: return "Pulse2Sweep_4005";
        case 0x4006: return "Pulse2TimerLo_4006";
        case 0x4007: return "Pulse2TimerHi_4007";
        case 0x4008: return "TriangleLinear_4008";
        case 0x400A: return "TriangleTimerLo_400A";
        case 0x400B: return "TriangleTimerHi_400B";
        case 0x400C: return "NoiseCtrl_400C";
        case 0x400E: return "NoisePeriod_400E";
        case 0x400F: return "NoiseLength_400F";
        case 0x4015: return "ApuStatus_4015";
        case 0x4014: return "OamDma_4014";
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

class WavWriter {
public:
    bool open(const char* path, uint32_t sampleRate) {
        fp_ = std::fopen(path, "wb");
        if (fp_ == nullptr) {
            std::perror("fopen");
            return false;
        }

        sampleRate_ = sampleRate;
        if (!writeHeader(0)) {
            std::perror("fwrite");
            std::fclose(fp_);
            fp_ = nullptr;
            return false;
        }
        return true;
    }

    bool writeSample(int16_t sample) {
        if (fp_ == nullptr) {
            return true;
        }
        if (!writeU16(static_cast<uint16_t>(sample))) {
            return false;
        }
        dataBytes_ += 2;
        return true;
    }

    bool close() {
        if (fp_ == nullptr) {
            return true;
        }
        if (std::fseek(fp_, 0, SEEK_SET) != 0) {
            std::perror("fseek");
            std::fclose(fp_);
            fp_ = nullptr;
            return false;
        }
        if (!writeHeader(dataBytes_)) {
            std::perror("fwrite");
            std::fclose(fp_);
            fp_ = nullptr;
            return false;
        }
        const bool ok = std::fclose(fp_) == 0;
        fp_ = nullptr;
        if (!ok) {
            std::perror("fclose");
        }
        return ok;
    }

    ~WavWriter() {
        close();
    }

private:
    FILE* fp_ = nullptr;
    uint32_t sampleRate_ = 44100;
    uint32_t dataBytes_ = 0;

    bool writeBytes(const char* bytes, size_t size) {
        return std::fwrite(bytes, 1, size, fp_) == size;
    }

    bool writeU16(uint16_t value) {
        const uint8_t bytes[2] = {
            static_cast<uint8_t>(value & 0xff),
            static_cast<uint8_t>((value >> 8) & 0xff),
        };
        return std::fwrite(bytes, 1, sizeof(bytes), fp_) == sizeof(bytes);
    }

    bool writeU32(uint32_t value) {
        const uint8_t bytes[4] = {
            static_cast<uint8_t>(value & 0xff),
            static_cast<uint8_t>((value >> 8) & 0xff),
            static_cast<uint8_t>((value >> 16) & 0xff),
            static_cast<uint8_t>((value >> 24) & 0xff),
        };
        return std::fwrite(bytes, 1, sizeof(bytes), fp_) == sizeof(bytes);
    }

    bool writeHeader(uint32_t dataBytes) {
        const uint16_t channels = 1;
        const uint16_t bitsPerSample = 16;
        const uint16_t blockAlign = channels * bitsPerSample / 8;
        const uint32_t byteRate = sampleRate_ * blockAlign;
        return writeBytes("RIFF", 4)
            && writeU32(36 + dataBytes)
            && writeBytes("WAVE", 4)
            && writeBytes("fmt ", 4)
            && writeU32(16)
            && writeU16(1)
            && writeU16(channels)
            && writeU32(sampleRate_)
            && writeU32(byteRate)
            && writeU16(blockAlign)
            && writeU16(bitsPerSample)
            && writeBytes("data", 4)
            && writeU32(dataBytes);
    }
};

bool saveFramePng(const char* path, void* pixels, int width, int height, int pitch) {
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

    const int result = IMG_SavePNG(surface, path);
    SDL_FreeSurface(surface);
    if (result != 0) {
        std::fprintf(stderr, "IMG_SavePNG Error: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

bool dumpVram(const char* path, Vtarunes_top* dut) {
    FILE* fp = std::fopen(path, "wb");
    if (fp == nullptr) {
        std::perror("fopen");
        return false;
    }

    auto* root = dut->rootp;
    for (int i = 0; i < 2048; i++) {
        const uint8_t value = root->tarunes_top__DOT__vram__DOT__mem[i];
        if (std::fwrite(&value, 1, 1, fp) != 1) {
            std::perror("fwrite");
            std::fclose(fp);
            return false;
        }
    }

    std::fclose(fp);
    return true;
}

void dumpPpuState(Vtarunes_top* dut) {
    auto* root = dut->rootp;
    auto oamField = [&](int sprite, int field) -> uint8_t {
        const uint32_t entry = root->tarunes_top__DOT__ppu_inst__DOT__oam[sprite];
        switch (field) {
        case 0:
            return static_cast<uint8_t>((entry >> 24) & 0xff);
        case 1:
            return static_cast<uint8_t>((entry >> 16) & 0xff);
        case 2:
            return static_cast<uint8_t>((entry >> 8) & 0xff);
        default:
            return static_cast<uint8_t>(entry & 0xff);
        }
    };

    std::printf(
        "PC=%04X PPUCTRL=%02X PPUMASK=%02X PPUSTATUS=%02X scanline=%d cycle=%d\n",
        root->tarunes_top__DOT__cpu_inst__DOT__reg_pc,
        root->tarunes_top__DOT__ppu_inst__DOT__reg_ctrl,
        root->tarunes_top__DOT__ppu_inst__DOT__reg_mask,
        root->tarunes_top__DOT__ppu_inst__DOT__reg_status,
        static_cast<int>(dut->scanline),
        static_cast<int>(dut->cycle));

    std::printf("OAM0:");
    for (int i = 0; i < 16; i++) {
        const int sprite = i / 4;
        const int field = i % 4;
        std::printf(" %02X", oamField(sprite, field));
    }
    std::printf("\n");

    int visible_oam = 0;
    for (int i = 0; i < 64; i++) {
        const uint8_t y = oamField(i, 0);
        if (y < 0xf0) {
            visible_oam++;
        }
    }
    std::printf("VisibleOAM=%d\n", visible_oam);

    std::printf("WRAM:");
    for (int addr : {0x0000, 0x0001, 0x0002, 0x001d, 0x071a, 0x0760, 0x076a, 0x0770, 0x0772, 0x0774, 0x0776, 0x0778, 0x0779}) {
        std::printf(" %04X=%02X", addr, root->tarunes_top__DOT__wram__DOT__mem[addr & 0x07ff]);
    }
    std::printf("\n");
}

void updateWindowTitle(SDL_Window* window, int frameCount, double fps) {
    char title[128];
    std::snprintf(title, sizeof(title), "tarunes | Frame %d | FPS %.1f", frameCount, fps);
    SDL_SetWindowTitle(window, title);
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    bool fast_mode = false;
    int frame_limit = -1;
    const char* vram_dump_path = nullptr;
    const char* ppu_write_log_path = nullptr;
    const char* audio_dump_path = "apu.wav";
    bool capture_hdmi = false;
    bool dump_state = false;
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "fast") == 0 || std::strcmp(argv[i], "--fast") == 0) {
            fast_mode = true;
        } else if (std::strcmp(argv[i], "--capture-hdmi") == 0) {
            capture_hdmi = true;
        } else if (std::strcmp(argv[i], "--frames") == 0 && i + 1 < argc) {
            frame_limit = std::atoi(argv[++i]);
        } else if (std::strcmp(argv[i], "--dump-vram") == 0 && i + 1 < argc) {
            vram_dump_path = argv[++i];
        } else if (std::strcmp(argv[i], "--log-ppu-writes") == 0 && i + 1 < argc) {
            ppu_write_log_path = argv[++i];
        } else if (std::strcmp(argv[i], "--dump-audio") == 0 && i + 1 < argc) {
            audio_dump_path = argv[++i];
        } else if (std::strcmp(argv[i], "--dump-state") == 0) {
            dump_state = true;
        }
    }

    Vtarunes_top* dut = new Vtarunes_top;
    VerilatedVcdC* tfp = nullptr;
    if (!fast_mode) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        dut->trace(tfp, 99);
        tfp->open("wave.vcd");
    }

    // SDL2の初期化
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    const int SCREEN_WIDTH = capture_hdmi ? HDMI_WIDTH : NES_WIDTH;
    const int SCREEN_HEIGHT = capture_hdmi ? HDMI_HEIGHT : NES_HEIGHT;
    const int WINDOW_WIDTH = capture_hdmi ? SCREEN_WIDTH : SCREEN_WIDTH * 2;
    const int WINDOW_HEIGHT = capture_hdmi ? SCREEN_HEIGHT : SCREEN_HEIGHT * 2;

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
    updateWindowTitle(window, 0, 0.0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
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

    // ピクセルバッファ (ARGB8888)
    std::vector<uint32_t> pixel_buffer(SCREEN_WIDTH * SCREEN_HEIGHT, 0);
    std::vector<uint32_t> last_frame_buffer(SCREEN_WIDTH * SCREEN_HEIGHT, 0);

    dut->clk = 0;
    dut->rst = 0;
    dut->frame_sync = 0;
    dut->sfc_data = 1;

    const int RESET_CYCLES = 4;
    int frame_count = 0;
    int prev_scanline = 0;
    bool prev_hdmi_frame_start = false;
    bool has_seen_hdmi_frame_start = false;
    bool running = true;
    bool has_completed_frame = false;
    uint8_t controller1_btns = 0;
    uint16_t sfc_shift = 0;
    uint8_t sfc_bit_index = 0;
    bool prev_sfc_clk = true;
    const uint32_t AUDIO_WAV_SAMPLE_RATE = 46875;
    bool frame_sync_pulse = false;
    uint64_t fps_sample_ticks = SDL_GetPerformanceCounter();
    int fps_sample_frame = frame_count;
    double current_fps = 0.0;
    const uint64_t PERF_FREQ = SDL_GetPerformanceFrequency();
    WavWriter audio_dump;
    if (!audio_dump.open(audio_dump_path, AUDIO_WAV_SAMPLE_RATE)) {
        return 1;
    }
    uint64_t cpu_cycle = 0;
    CpuTracer cpu_tracer;
    FILE* ppu_write_log = nullptr;
    if (ppu_write_log_path != nullptr) {
        ppu_write_log = std::fopen(ppu_write_log_path, "w");
        if (ppu_write_log == nullptr) {
            std::perror("fopen");
            return 1;
        }
    }

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

        dut->frame_sync = frame_sync_pulse ? 1 : 0;
        frame_sync_pulse = false;

        if (dut->sfc_latch) {
            sfc_shift = 0;
            sfc_shift |= ((controller1_btns >> 1) & 1) << 0; // B
            sfc_shift |= ((controller1_btns >> 2) & 1) << 2; // Select
            sfc_shift |= ((controller1_btns >> 3) & 1) << 3; // Start
            sfc_shift |= ((controller1_btns >> 4) & 1) << 4; // Up
            sfc_shift |= ((controller1_btns >> 5) & 1) << 5; // Down
            sfc_shift |= ((controller1_btns >> 6) & 1) << 6; // Left
            sfc_shift |= ((controller1_btns >> 7) & 1) << 7; // Right
            sfc_shift |= ((controller1_btns >> 0) & 1) << 8; // A
            sfc_bit_index = 0;
        } else if (prev_sfc_clk && !dut->sfc_clk && sfc_bit_index != 15) {
            sfc_bit_index++;
        }
        dut->sfc_data = !((sfc_shift >> sfc_bit_index) & 1);
        prev_sfc_clk = dut->sfc_clk;

        // 1サイクルシミュレーション
        for (int half = 0; half < 2; half++) {
            if (Verilated::time() == RESET_CYCLES) {
                dut->rst = 1;
            }

            dut->clk = half;
            dut->eval();
            if (tfp != nullptr) {
                tfp->dump(Verilated::time());
            }
            Verilated::timeInc(1);
        }
        if (dut->rst) {
            cpu_cycle++;
        }
        if (!fast_mode) {
            cpu_tracer.trace(dut, frame_count, cpu_cycle);
        }
        if (ppu_write_log != nullptr) {
            auto* ppu_regbus = dut->rootp->__PVT__tarunes_top__DOT__cpu_ppubus;
            if (ppu_regbus->wen && (ppu_regbus->addr == 6 || ppu_regbus->addr == 7)) {
                std::fprintf(
                    ppu_write_log,
                    "fr=%d sl=%d cy=%d pc=%04X reg=%u data=%02X reg_v=%04X ppu_addr_cpu=%04X\n",
                    frame_count,
                    static_cast<int>(dut->scanline),
                    static_cast<int>(dut->cycle),
                    dut->rootp->tarunes_top__DOT__cpu_inst__DOT__reg_pc,
                    static_cast<unsigned>(ppu_regbus->addr),
                    static_cast<unsigned>(ppu_regbus->wdata),
                    dut->rootp->tarunes_top__DOT__ppu_inst__DOT__reg_v,
                    dut->rootp->tarunes_top__DOT__ppu_inst__DOT__ppu_addr_cpu);
            }
        }
        if (dut->rootp->tarunes_top__DOT__audio_write_pulse) {
            int centered = static_cast<int>(dut->audio_sample) - 128;
            int16_t sample = static_cast<int16_t>(centered * 192);
            if (!audio_dump.writeSample(sample)) {
                std::fprintf(stderr, "Failed to write WAV sample to %s\n", audio_dump_path);
                running = false;
            }
        }

        bool frame_completed = false;
        bool hdmi_frame_start = false;
        if (capture_hdmi) {
            bool hdmi_de = dut->hdmi_de;
            int hdmi_x = dut->hdmi_video_x;
            int hdmi_y = dut->hdmi_video_y;
            bool hdmi_frame_start_now = hdmi_de && hdmi_x == 0 && hdmi_y == 0;
            hdmi_frame_start = hdmi_frame_start_now && !prev_hdmi_frame_start;
            frame_completed = hdmi_frame_start && has_seen_hdmi_frame_start;
            frame_sync_pulse = hdmi_frame_start;
        } else {
            int scanline = dut->scanline;
            frame_completed = prev_scanline == 261 && scanline == 0;
            frame_sync_pulse = dut->rootp->tarunes_top__DOT__ppu_frame_wait;
        }

        if (frame_completed) {
            frame_count++;
            uint64_t now = SDL_GetPerformanceCounter();
            double fps_sample_seconds =
                static_cast<double>(now - fps_sample_ticks) / static_cast<double>(PERF_FREQ);
            if (fps_sample_seconds >= 0.5) {
                current_fps = static_cast<double>(frame_count - fps_sample_frame) / fps_sample_seconds;
                fps_sample_frame = frame_count;
                fps_sample_ticks = now;
            }
            updateWindowTitle(window, frame_count, current_fps);
            if (!fast_mode) {
                printf("Frame %d completed\n", frame_count);
            }
            std::copy(pixel_buffer.begin(), pixel_buffer.end(), last_frame_buffer.begin());
            has_completed_frame = true;
            if (frame_limit >= 0 && frame_count >= frame_limit) {
                running = false;
            }

            // ピクセルバッファをテクスチャにコピーして表示
            SDL_UpdateTexture(texture, NULL, pixel_buffer.data(), SCREEN_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

        if (capture_hdmi) {
            int hdmi_x = dut->hdmi_video_x;
            int hdmi_y = dut->hdmi_video_y;
            bool hdmi_de = dut->hdmi_de;
            int pixel_r = dut->hdmi_r;
            int pixel_g = dut->hdmi_g;
            int pixel_b = dut->hdmi_b;
            if (hdmi_de && hdmi_x < SCREEN_WIDTH && hdmi_y < SCREEN_HEIGHT) {
                pixel_buffer[hdmi_y * SCREEN_WIDTH + hdmi_x] =
                    0xFF000000 |
                    ((pixel_r & 0xFF) << 16) |
                    ((pixel_g & 0xFF) << 8) |
                    (pixel_b & 0xFF);
            }
            prev_hdmi_frame_start = hdmi_de && hdmi_x == 0 && hdmi_y == 0;
        } else {
            int scanline = dut->scanline;
            int cycle = dut->cycle;
            if (cycle < SCREEN_WIDTH && scanline < SCREEN_HEIGHT) {
                uint8_t palette_index = dut->pixel_index & 0x3F;
                pixel_buffer[scanline * SCREEN_WIDTH + cycle] =
                    argbFromRgb24(NES_PALETTE_RGB[palette_index]);
            }
            prev_scanline = scanline;
        }
        if (hdmi_frame_start) {
            has_seen_hdmi_frame_start = true;
        }
    }

    if (!fast_mode) {
        printf("Simulation finished after %d frames\n", frame_count);
    }
    if (saveFramePng(
            "last_frame.png",
            has_completed_frame ? static_cast<void*>(last_frame_buffer.data()) : static_cast<void*>(pixel_buffer.data()),
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SCREEN_WIDTH * sizeof(uint32_t))) {
        if (!fast_mode) {
            printf("Saved last frame to last_frame.png\n");
        }
    }
    if (vram_dump_path != nullptr && !dumpVram(vram_dump_path, dut)) {
        fprintf(stderr, "Failed to dump VRAM to %s\n", vram_dump_path);
    }
    if (dump_state) {
        dumpPpuState(dut);
    }
    if (!audio_dump.close()) {
        std::fprintf(stderr, "Failed to finalize WAV file %s\n", audio_dump_path);
    } else if (!fast_mode) {
        std::printf("Saved audio to %s\n", audio_dump_path);
    }

    // クリーンアップ
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    if (tfp != nullptr) {
        tfp->close();
        delete tfp;
    }
    if (ppu_write_log != nullptr) {
        std::fclose(ppu_write_log);
    }
    delete dut;
    return 0;
}
