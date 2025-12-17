VERILATOR      := verilator
VERILATOR_FLAGS := -Wall --trace --Wno-fatal -GPROM_PATH=\"helloworld_prg.hex\"

RTL_DIR  := target
VERYL_PROJ:= tarunes
TB_CPP := src/tb_top.cpp
TOP    := $(VERYL_PROJ)_top

SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)

all: build

veryl-fmt:
	veryl fmt

veryl-build: veryl-fmt
	veryl build

build: veryl-build
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--cc \
		-f $(VERYL_PROJ).f \
		--top-module $(TOP) \
		--exe $(TB_CPP) \
		-I$(RTL_DIR) \
		-CFLAGS "-std=c++17 $(SDL_CFLAGS)" \
		-LDFLAGS "$(SDL_LDFLAGS)"

	make -C obj_dir -f V$(TOP).mk

run:
	./obj_dir/V$(TOP)

clean:
	rm -rf obj_dir target *.vcd

.PHONY: all build run clean veryl-fmt veryl-build
