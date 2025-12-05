#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcore_top.h"

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

    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " PROM_FILE_PATH CROM_FILE_PATH [CYCLE]" << std::endl;
        return 1;
    }

    // 引数からファイルパスを取得
    std::string prom_file_path = argv[1];
    std::string crom_file_path = argv[2];

    // 絶対パスにする
    try {
        prom_file_path = fs::absolute(prom_file_path).string();
        crom_file_path = fs::absolute(crom_file_path).string();
    } catch (const std::exception& e) {
        std::cerr << "Invalid memory file path : " << e.what() << std::endl;
        return 1;
    }

    // 指定サイクル数
    uint64_t cycles = 0;
    if (argc >= 3) {
        cycles = std::stoull(argv[3]);
    }

    // 環境変数を設定
    const char* original_prom_env = std::getenv("PROM_FILE_PATH");
    const char* original_crom_env = std::getenv("CROM_FILE_PATH");
    setenv("PROM_FILE_PATH", prom_file_path.c_str(), 1);
    setenv("CROM_FILE_PATH", crom_file_path.c_str(), 1);

    // DUT 生成
    Vcore_top* dut = new Vcore_top;

    // 波形出力
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("dump.vcd");

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
    if (original_prom_env)
        setenv("PROM_FILE_PATH", original_prom_env, 1);
    if (original_crom_env)
        setenv("CROM_FILE_PATH", original_crom_env, 1);

    return 0;
}
