VERILATOR      := verilator
VERILATOR_FLAGS := -Wall --trace --Wno-fatal -GPROM_PATH=\"helloworld_prg.hex\"

RTL_DIR  := target
RTL_SRCS := $(wildcard $(RTL_DIR)/*.sv)

TB_CPP := src/tb_top.cpp
TOP    := tarunes_top

all: build

veryl-fmt:
	veryl fmt

veryl-build: veryl-fmt
	veryl build

build: veryl-build
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--cc $(RTL_SRCS) \
		--top-module $(TOP) \
		--exe $(TB_CPP) \
		-I$(RTL_DIR) \
		-CFLAGS "-std=c++17"

	make -C obj_dir -f V$(TOP).mk

run:
	./obj_dir/V$(TOP)

clean:
	rm -rf obj_dir *.vcd

.PHONY: all build run clean veryl-fmt veryl-build
