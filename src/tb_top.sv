`timescale 1ns/1ps

module tb_top;

    reg clk = 0;
    reg rst = 0;
    reg [7:0] controller1_btns = 8'h00;
    reg [7:0] controller2_btns = 8'h00;

    tarunes_top #(
        .PROM_PATH("helloworld_prg.hex")
        )
        dut (
        .clk(clk),
        .rst(rst),
        .controller1_btns(controller1_btns),
        .controller2_btns(controller2_btns)
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
