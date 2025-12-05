VERILATOR_FLAGS := -Wall --trace --Wno-fatal --top-module tb_top --binary

RTL_DIR := ../core/target
RTL_SRCS := $(wildcard $(RTL_DIR)/*.sv)

all: build

build:
	verilator $(VERILATOR_FLAGS) \
		-I$(RTL_DIR) \
		tb_top.sv $(RTL_SRCS)

run: build
	./tb_top     # ← Verilator が自動生成する実行ファイル名

clean:
	rm -rf obj_dir *.vcd tb_top

.PHONY: all build run clean
