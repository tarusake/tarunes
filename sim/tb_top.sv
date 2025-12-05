`timescale 1ns/1ps

module tb_top;

    reg clk = 0;
    reg rst = 1;

    core_top dut (
        .clk(clk),
        .rst(rst)
    );

    // クロック生成（10ns = 100MHz）
    always #5 clk = ~clk;

    initial begin

        // --- ROM の内容をセット（0xFFFC→0x8000, opcode = NOP） ---

        // リセットベクタ (FFFC = 00h, FFFD = 80h)
        dut.prom.mem[16'h7FFC] = 8'h00;
        dut.prom.mem[16'h7FFD] = 8'h80;

        // 実行コード領域 0x8000 に NOP を置く
        dut.prom.mem[16'h0000] = 8'hEA;

        // リセット解除
        #20 rst = 0;

        // 必要なサイクルだけ回す
        #200;

        $display("PC = %h", dut.cpu_inst.reg_pc);
        $finish;
    end

endmodule
