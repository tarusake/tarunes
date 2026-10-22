VERILATOR      := verilator
ROM            ?= helloworld
ENABLE_APU     ?= 1
NES_ROM        := $(ROM).nes
PRG_HEX        := $(ROM)_prg.hex
CHR_HEX        := $(ROM)_chr.hex
VERILATOR_FLAGS := -Wall --trace-fst --Wno-fatal -GPROM_PATH=\"$(PRG_HEX)\" -GCROM_PATH=\"$(CHR_HEX)\" -GENABLE_APU=$(ENABLE_APU)

RTL_DIR  := target
VERYL_PROJ:= tarunes
TB_CPP := src/tb_top.cpp
TOP    := $(VERYL_PROJ)_top

SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
SDL_IMAGE_CFLAGS := $(shell pkg-config --cflags SDL2_image 2>/dev/null)
SDL_IMAGE_LDFLAGS := $(shell pkg-config --libs SDL2_image 2>/dev/null)

all: build

veryl-fmt:
	veryl fmt

veryl-build: veryl-fmt
	veryl build
	perl -0pi -e 's/\)\s*;\n    typedef struct packed/\) \/\*synthesis syn_ramstyle="registers"\*\/ ;\n    typedef struct packed/' target/ppu.sv

test:
	veryl test

test-controller:
	veryl test --test test_controller_serial_read --no-capture

test-oam-dma:
	veryl test --test test_oam_dma_transfer --no-capture

test-bus-cpu:
	veryl test --test test_bus_cpu_decode --no-capture

test-cpu:
	veryl test --test test_cpu_program_execution --no-capture

test-cpu-alu:
	veryl test --test test_cpu_alu_and_flags --no-capture

test-cpu-addressing:
	veryl test --test test_cpu_addressing_modes --no-capture

test-cpu-branch:
	veryl test --test test_cpu_branch_conditions --no-capture

test-cpu-stack:
	veryl test --test test_cpu_stack_and_subroutine --no-capture

test-cpu-interrupt:
	veryl test --test test_cpu_interrupts --no-capture

test-bus-ppu:
	veryl test --test test_bus_ppu_decode --no-capture

test-memory:
	veryl test --test test_memory_read_write --no-capture

test-i2s:
	veryl test --test test_i2s_frame --no-capture

test-sfc-controller:
	veryl test --test test_sfc_controller_poll --no-capture

test-hdmi-scaler:
	veryl test --test test_hdmi_480p_timing_and_scaling --no-capture

test-apu:
	veryl test --test test_apu_registers_and_status --no-capture

test-apu-pulse:
	veryl test --test test_apu_pulse_timing_and_duty --no-capture

rom: $(PRG_HEX) $(CHR_HEX)

$(PRG_HEX) $(CHR_HEX): $(NES_ROM) nes2hex.py
	./nes2hex.py $(NES_ROM)

build: veryl-build rom
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--cc \
		-f $(VERYL_PROJ).f \
		--top-module $(TOP) \
		--exe $(TB_CPP) \
		-I$(RTL_DIR) \
		-CFLAGS "-std=c++17 $(SDL_CFLAGS) $(SDL_IMAGE_CFLAGS)" \
		-LDFLAGS "$(SDL_LDFLAGS) $(SDL_IMAGE_LDFLAGS)"

	make -C obj_dir -f V$(TOP).mk

run: build
	./obj_dir/V$(TOP) $(ARGS)

clean:
	rm -rf obj_dir target *.vcd *.fst *.fst.hier

.PHONY: all build run clean veryl-fmt veryl-build test test-controller test-oam-dma test-bus-cpu test-cpu test-cpu-alu test-cpu-addressing test-cpu-branch test-cpu-stack test-cpu-interrupt test-bus-ppu test-memory test-i2s test-sfc-controller test-hdmi-scaler test-apu test-apu-pulse rom
