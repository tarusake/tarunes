# Tang Nano 20K bring-up

This directory contains a first FPGA bring-up shell for Tang Nano 20K.

## Files

- `tang_nano_20k_top.sv`: board-level wrapper around `tarunes_top`
- `tang_nano_20k.cst`: pin constraints for the Tang Nano 20K HDMI pins
- `tang_nano_20k.sdc`: timing constraints
- `gowin_build.tcl`: Gowin EDA batch build script

## Build

From the repository root:

```bash
make tangnano20k ROM=helloworld
```

If running `gowin_build.tcl` directly with `gw_sh`, generate the ROM hex files
first:

```bash
make rom ROM=helloworld
```

The Tcl script creates the Gowin build in batch mode, adds the generated Veryl
RTL and board files, and runs synthesis, place-and-route, and bitstream
generation. The result is written to
`tarunes_TangNano20k/impl/pnr/tarunes_tangnano20k.fs`.

The top module must be `tang_nano_20k_top`.

The wrapper writes the NES 256x240 palette-index pixel stream into a 6-bit
framebuffer and reads it as a centered 2x image inside 720x480p timing. The
generated `gowin_pll_27_to_135` rPLL IP converts the board's 27 MHz clock to a
135 MHz serial clock. `CLKDIV` then derives a phase-aligned 27 MHz pixel clock
for the NES core and `OSER10`.

`src/gowin_tmds_tx.veryl` implements TMDS encoding. The board bridge uses only
the `OSER10`, `CLKDIV`, and `TLVDS_OBUF` device primitives, so no encrypted DVI
IP is required.

The NES core also uses `I_clk` as its 27 MHz clock. The HDMI scanout sends a
frame-start pulse into the core. If the PPU reaches the end of its frame before
that pulse, the PPU waits internally and the rest of the NES core stalls with it.
This keeps the PPU frame boundary synchronized to the DVI/HDMI frame boundary.

Top-level board ports are `I_clk`, `I_rst`, `I_key`, `O_tmds_clk_p`,
`O_tmds_clk_n`, `O_tmds_data_p[2:0]`, `O_tmds_data_n[2:0]`, and
`O_i2s_bclk`, `O_i2s_lrck`, `O_i2s_din`, `O_pa_sd_en`, and `O_led[5:0]`.
`I_rst` is S1 and `I_key` is S2; both buttons are active-high on the board. S1
resets the core. S2 is reserved for a future controller input. `O_led[5]` and
`O_led[4]` show CPU and video-frame activity, and `O_led[3:0]` shows
`debug_cpu_pc[15:12]`.

Audio is currently disabled because the Gowin EDA RAM inference pass crashes
while synthesizing the APU audio buffer. The I2S and amplifier-enable outputs
are therefore held low.

The batch script adds these files automatically:

- `tarunes_TangNano20k/src/tang_nano_20k_top.sv`
- `tarunes_TangNano20k/src/gowin_tmds_output.sv`
- `tarunes_TangNano20k/src/gowin_rpll/gowin_pll_27_to_135.v`
- `target/gowin_tmds_tx.sv`
- `target/*.sv`
