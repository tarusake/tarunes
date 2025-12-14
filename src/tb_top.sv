`timescale 1ns/1ps

module tb_top;

    reg clk = 0;
    reg rst = 0;

    tarunes_top dut (
        .clk(clk),
        .rst(rst)
    );

    // クロック生成（10ns = 100MHz）
    always #5 clk = ~clk;

    initial begin
        $dumpfile("wave.vcd");
        $dumpvars(0, tb_top);

        // リセットベクタ (FFFC = 00h, FFFD = 80h)
        dut.prom.mem[16'h7FFC] = 8'h00;
        dut.prom.mem[16'h7FFD] = 8'h80;

        // SEI
        dut.prom.mem[16'h0000] = 8'h78; 
        
        // LDX #$FF
        dut.prom.mem[16'h0001] = 8'hA2;
        dut.prom.mem[16'h0002] = 8'hFF;

        // TXS
        dut.prom.mem[16'h0003] = 8'h9A;

        // LDA #$00
        dut.prom.mem[16'h0004] = 8'hA9;
        dut.prom.mem[16'h0005] = 8'h00;

        // リセット解除
        #20 rst = 1;

        // 必要なサイクルだけ回す
        #200;

        $display("PC = %h", dut.cpu_inst.reg_pc);
        $finish;
    end

endmodule
