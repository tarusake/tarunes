#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcore_top.h"
#include "Vcore_top___024root.h"   // ← これが必要！！

namespace fs = std::filesystem;

// Veryl util.sv の DPI-C 関数を提供
extern "C" const char* get_env_value(const char* key) {
    const char* value = std::getenv(key);
    return value ? value : "";
}

// Verilator 時間カウンタ
vluint64_t main_time = 0;
double sc_time_stamp() { return main_time; }

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " MEMORY_FILE_PATH [CYCLE]" << std::endl;
        return 1;
    }

    // 引数からファイルパスを取得
    std::string memory_file_path = argv[1];

    // 絶対パスにする
    memory_file_path = fs::absolute(memory_file_path).string();

    // 指定サイクル数
    uint64_t cycles = 0;
    if (argc >= 3) {
        cycles = std::stoull(argv[2]);
    }

    // 環境変数を設定
    const char* original_env = std::getenv("MEMORY_FILE_PATH");
    setenv("MEMORY_FILE_PATH", memory_file_path.c_str(), 1);

    // DUT 生成
    Vcore_top* dut = new Vcore_top;

    // 波形出力
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("dump.vcd");

    // memory
    // dut->rootp->core_top__DOT__prom__DOT__mem[0x7FFC] = 0x00;
    // dut->rootp->core_top__DOT__prom__DOT__mem[0x7FFD] = 0x80;
    // dut->rootp->core_top__DOT__prom__DOT__mem[0x0000] = 0xff;

    // Reset シーケンス
    dut->clk = 0;
    dut->rst = 0;
    for (int i = 0; i < 4; i++) {
        dut->eval();
        main_time++;
        tfp->dump(main_time);
        dut->clk = !dut->clk;
    }

    dut->rst = 1;  // Reset 解除

    // LOOP
    for (uint64_t i = 0; !Verilated::gotFinish() && (cycles == 0 || i < cycles); i++) {
        dut->clk = 0;
        dut->eval();
        main_time++;
        tfp->dump(main_time);

        dut->clk = 1;
        dut->eval();
        main_time++;
        tfp->dump(main_time);
    }

    dut->final();
    tfp->close();

    // 環境変数を元に戻す
    if (original_env)
        setenv("MEMORY_FILE_PATH", original_env, 1);

    return 0;
}
