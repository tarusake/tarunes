`default_nettype none

module tang_nano_20k_top #(
    parameter string PROM_PATH = "rom_prg.hex",
    parameter string CROM_PATH = "rom_chr.hex"
) (
    input  wire       I_clk,
    input  wire       I_rst,
    input  wire       I_key,
    output wire       O_tmds_clk_p,
    output wire       O_tmds_clk_n,
    output wire [2:0] O_tmds_data_p,
    output wire [2:0] O_tmds_data_n,
    output wire       O_i2s_bclk,
    output wire       O_i2s_lrck,
    output wire       O_i2s_din,
    output wire       O_pa_sd_en,
    output wire [5:0] O_led,
    output wire [15:0] logic_analizer
);

    wire clk27;
    wire clk135;
    wire pll_lock;

    gowin_pll_27_to_135 hdmi_pll (
        .clkin(I_clk),
        .clkout(clk135),
        .lock(pll_lock)
    );

    // OSER10のPCLKは、FCLKを同じクロック源から5分周して位相を揃える。
    CLKDIV pixel_clock_divider (
        .RESETN(pll_lock && !I_rst),
        .HCLKIN(clk135),
        .CLKOUT(clk27),
        .CALIB(1'b1)
    );
    defparam pixel_clock_divider.DIV_MODE = "5";
    defparam pixel_clock_divider.GSREN = "false";

    // 基板のリセットボタンは active-high。S1 押下中は I_rst=1 になる。
    // リセット解除は clk27 に同期してから後段へ渡す。
    // PLLが安定するまでは、TMDSシリアライザを含む回路全体をリセット状態に保つ。
    wire rst_n_async = !I_rst && pll_lock;
    logic [1:0] rst_n_sync = 2'b00;

    // リセットは即時アサートし、解除だけ clk27 の2段同期後に行う。
    always_ff @(posedge clk27) begin
        if (!rst_n_async) begin
            rst_n_sync <= 2'b00;
        end else begin
            rst_n_sync <= {rst_n_sync[0], 1'b1};
        end
    end

    wire rst_n = rst_n_sync[1];
    wire sfc_data = 1'b1;
    wire sfc_latch;
    wire sfc_clk;

    logic [7:0] hdmi_r;
    logic [7:0] hdmi_g;
    logic [7:0] hdmi_b;
    logic       hdmi_hsync;
    logic       hdmi_vsync;
    logic       hdmi_de;
    logic       hdmi_de_d;
    logic [9:0] hdmi_video_y;

    wire hdmi_frame_start = hdmi_de && !hdmi_de_d && (hdmi_video_y == 10'd0);

    logic [15:0] debug_cpu_pc;
    logic [7:0] debug_palette0;
    logic [7:0] debug_palette1;
    logic [7:0] debug_palette2;
    logic [7:0] debug_palette3;
    logic [4:0] debug_bg_palette_addr;
    logic [7:0] debug_bg_palette_data;
    logic       debug_palette_write;
    logic [4:0] debug_palette_write_addr;
    logic [7:0] debug_palette_write_data;
    logic [15:0] debug_cpu_pc_d;
    logic [23:0] cpu_activity_counter;
    logic [7:0] hdmi_frame_counter;

    tarunes_top #(
        .PROM_PATH(PROM_PATH),
        .CROM_PATH(CROM_PATH),
        .ENABLE_APU(1'b1),
        .USE_CARTRIDGE(1'b0)
    ) core (
        .clk(clk27),
        .rst(rst_n),
        .frame_sync(hdmi_frame_start),
        .sfc_data(sfc_data),
        .sfc_latch(sfc_latch),
        .sfc_clk(sfc_clk),
        .scanline(),
        .cycle(),
        .pixel_index(),
        .hdmi_r(hdmi_r),
        .hdmi_g(hdmi_g),
        .hdmi_b(hdmi_b),
        .hdmi_hsync(hdmi_hsync),
        .hdmi_vsync(hdmi_vsync),
        .hdmi_de(hdmi_de),
        .hdmi_video_x(),
        .hdmi_video_y(hdmi_video_y),
        .audio_sample(),
        .i2s_bclk(O_i2s_bclk),
        .i2s_lrck(O_i2s_lrck),
        .i2s_dout(O_i2s_din),
        .debug_cpu_pc(debug_cpu_pc),
        .logic_analizer(logic_analizer),
        .cart_cpu_addr(),
        .cart_cpu_data_i(8'h00),
        .cart_cpu_data_o(),
        .cart_cpu_data_oe(),
        .cart_cpu_rnw(),
        .cart_cpu_romsel_n(),
        .cart_cpu_m2(),
        .cart_ppu_addr(),
        .cart_ppu_data_i(8'h00),
        .cart_ppu_data_o(),
        .cart_ppu_data_oe(),
        .cart_ppu_rd_n(),
        .cart_ppu_wr_n(),
        .debug_palette0(debug_palette0),
        .debug_palette1(debug_palette1),
        .debug_palette2(debug_palette2),
        .debug_palette3(debug_palette3),
        .debug_bg_palette_addr(debug_bg_palette_addr),
        .debug_bg_palette_data(debug_bg_palette_data),
        .debug_palette_write(debug_palette_write),
        .debug_palette_write_addr(debug_palette_write_addr),
        .debug_palette_write_data(debug_palette_write_data)
    );

    always_ff @(posedge clk27) begin
        if (!rst_n) begin
            hdmi_de_d <= 1'b0;
            hdmi_frame_counter <= '0;
        end else begin
            hdmi_de_d <= hdmi_de;
            if (hdmi_frame_start) begin
                hdmi_frame_counter <= hdmi_frame_counter + 1'b1;
            end
        end
    end

`ifdef VERILATOR
    assign O_tmds_clk_p = clk27;
    assign O_tmds_clk_n = ~clk27;
    assign O_tmds_data_p = 3'b000;
    assign O_tmds_data_n = 3'b111;
`else
    gowin_tmds_output hdmi_out (
        .reset_n(rst_n), .serial_clk(clk135), .pixel_clk(clk27),
        .vsync(hdmi_vsync), .hsync(hdmi_hsync), .data_enable(hdmi_de),
        .red(hdmi_r), .green(hdmi_g), .blue(hdmi_b),
        .tmds_clk_p(O_tmds_clk_p), .tmds_clk_n(O_tmds_clk_n),
        .tmds_data_p(O_tmds_data_p), .tmds_data_n(O_tmds_data_n)
    );
`endif

    always_ff @(posedge clk27) begin
        if (!rst_n) begin
            debug_cpu_pc_d <= '0;
            cpu_activity_counter <= '0;
        end else begin
            debug_cpu_pc_d <= debug_cpu_pc;
            if (debug_cpu_pc != debug_cpu_pc_d) begin
                cpu_activity_counter <= cpu_activity_counter + 1'b1;
            end
        end
    end

    wire cpu_activity_blink = cpu_activity_counter[20];

    assign O_pa_sd_en = rst_n;
    assign O_led = {cpu_activity_blink, hdmi_frame_counter[5], debug_cpu_pc[15:12]};

endmodule

`default_nettype wire
