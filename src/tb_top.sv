`timescale 1ns/1ps

module tb_top;

    reg clk = 0;
    reg rst = 0;
    reg frame_sync = 1;
    reg sfc_data = 1'b1;
    wire sfc_latch;
    wire sfc_clk;

    tarunes_top #(
        .PROM_PATH("helloworld_prg.hex")
        )
        dut (
        .clk(clk),
        .rst(rst),
        .frame_sync(frame_sync),
        .sfc_data(sfc_data),
        .sfc_latch(sfc_latch),
        .sfc_clk(sfc_clk)
    );

    // クロック生成（10ns = 100MHz）
    always #5 clk = ~clk;

    initial begin
        $dumpfile("wave.vcd");
        $dumpvars(0, tb_top);

        // リセット解除
        #20 rst = 1;

        // 必要なサイクルだけ回す
        #10000;

        $display("PC = %h", dut.cpu_inst.reg_pc);
        $finish;
    end

endmodule
