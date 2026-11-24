`default_nettype none

// TMDS符号化はVeryl側で行い、ここではGowin固有プリミティブだけを接続する。
module gowin_tmds_output (
    input  wire       reset_n,
    input  wire       serial_clk,
    input  wire       pixel_clk,
    input  wire       vsync,
    input  wire       hsync,
    input  wire       data_enable,
    input  wire [7:0] red,
    input  wire [7:0] green,
    input  wire [7:0] blue,
    output wire       tmds_clk_p,
    output wire       tmds_clk_n,
    output wire [2:0] tmds_data_p,
    output wire [2:0] tmds_data_n
);
    wire reset = !reset_n;
    wire [9:0] encoded_r;
    wire [9:0] encoded_g;
    wire [9:0] encoded_b;
    wire serial_r;
    wire serial_g;
    wire serial_b;

    // Verylのreset型はactive-lowなので、解除時に1となるreset_nをそのまま渡す。
    tarunes_tmds_encoder encoder_r (.pixel_clk(pixel_clk), .rst(reset_n), .video_data(red),   .data_enable(data_enable), .control(2'b00),           .tmds_data(encoded_r));
    tarunes_tmds_encoder encoder_g (.pixel_clk(pixel_clk), .rst(reset_n), .video_data(green), .data_enable(data_enable), .control(2'b00),           .tmds_data(encoded_g));
    tarunes_tmds_encoder encoder_b (.pixel_clk(pixel_clk), .rst(reset_n), .video_data(blue),  .data_enable(data_enable), .control({vsync, hsync}), .tmds_data(encoded_b));

    OSER10 serializer_r (.D0(encoded_r[0]), .D1(encoded_r[1]), .D2(encoded_r[2]), .D3(encoded_r[3]), .D4(encoded_r[4]), .D5(encoded_r[5]), .D6(encoded_r[6]), .D7(encoded_r[7]), .D8(encoded_r[8]), .D9(encoded_r[9]), .PCLK(pixel_clk), .FCLK(serial_clk), .RESET(reset), .Q(serial_r));
    defparam serializer_r.GSREN = "false";
    defparam serializer_r.LSREN = "true";
    OSER10 serializer_g (.D0(encoded_g[0]), .D1(encoded_g[1]), .D2(encoded_g[2]), .D3(encoded_g[3]), .D4(encoded_g[4]), .D5(encoded_g[5]), .D6(encoded_g[6]), .D7(encoded_g[7]), .D8(encoded_g[8]), .D9(encoded_g[9]), .PCLK(pixel_clk), .FCLK(serial_clk), .RESET(reset), .Q(serial_g));
    defparam serializer_g.GSREN = "false";
    defparam serializer_g.LSREN = "true";
    OSER10 serializer_b (.D0(encoded_b[0]), .D1(encoded_b[1]), .D2(encoded_b[2]), .D3(encoded_b[3]), .D4(encoded_b[4]), .D5(encoded_b[5]), .D6(encoded_b[6]), .D7(encoded_b[7]), .D8(encoded_b[8]), .D9(encoded_b[9]), .PCLK(pixel_clk), .FCLK(serial_clk), .RESET(reset), .Q(serial_b));
    defparam serializer_b.GSREN = "false";
    defparam serializer_b.LSREN = "true";
    TLVDS_OBUF output_r   (.I(serial_r),         .O(tmds_data_p[2]), .OB(tmds_data_n[2]));
    TLVDS_OBUF output_g   (.I(serial_g),         .O(tmds_data_p[1]), .OB(tmds_data_n[1]));
    TLVDS_OBUF output_b   (.I(serial_b),         .O(tmds_data_p[0]), .OB(tmds_data_n[0]));
    // TMDSクロックはピクセルクロックをそのまま差動出力する。
    // データだけをOSER10で10:1シリアライズする構成はGowinの実績例と同じ。
    TLVDS_OBUF output_clk (.I(pixel_clk), .O(tmds_clk_p), .OB(tmds_clk_n));
endmodule

`default_nettype wire
