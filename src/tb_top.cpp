#include "Vtarunes_top.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Verilated::traceEverOn(true);

    Vtarunes_top* dut = new Vtarunes_top;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("wave.vcd");

    dut->clk = 0;
    dut->rst = 0;

    const int RESET_CYCLES = 4;
    const int SIM_CYCLES   = 1000;

    for (int cycle = 0; cycle < SIM_CYCLES; cycle++) {

        if (cycle == RESET_CYCLES)
            dut->rst = 1;

        // Low
        dut->clk = 0;
        dut->eval();
        tfp->dump(Verilated::time());
        Verilated::timeInc(1);

        // High
        dut->clk = 1;
        dut->eval();
        tfp->dump(Verilated::time());
        Verilated::timeInc(1);
    }

    tfp->close();
    delete dut;
    delete tfp;
    return 0;
}
