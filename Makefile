VERILATOR_FLAGS := -Wall --trace --Wno-fatal --top-module tb_top --binary

RTL_DIR := target
RTL_SRCS := $(wildcard $(RTL_DIR)/*.sv)

all: build

veryl-fmt:
	veryl fmt

veryl-build: veryl-fmt
	veryl build

build: veryl-build
	verilator $(VERILATOR_FLAGS) \
		-I$(RTL_DIR) \
		src/tb_top.sv $(RTL_SRCS)

run: 
	./obj_dir/Vtb_top

clean:
	rm -rf obj_dir *.vcd tb_top

.PHONY: all build run clean veryl-fmt veryl-build
