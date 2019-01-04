
SIM_DIR = sim
SRC_DIR = src

ASM_DIR = asm

C_DIR = c

TOP_SRC_NAME = core


VERILATOR_DIR = verilator

VERILATOR_OBJ_DIR = obj_dir

MICROCODE = src/cs_mapper_mod.v src/microcode_mod.v src/metadata_vector.txt src/opcode_vector.txt src/subop_vector.txt

SRCS = $(wildcard $(SRC_DIR)/*.v)

VERILATOR_CPP_FLAGS = -g -std=c++14 -pthread -I/usr/src/gtest

VERILATOR_FLAGS = --trace -CFLAGS '$(VERILATOR_CPP_FLAGS)' -LDFLAGS -lpthread

TOP_SRC = $(TOP_SRC_NAME).v

GTEST_DIR = /usr/src/gtest

MICROCODE_FILES = $(notdir $(MICROCODE))

FILES = $(basename $(notdir $(wildcard $(SIM_DIR)/*.v)))

ASM_SRCS = $(shell find $(ASM_DIR) -name '*.s')

ASM_OBJS = $(addsuffix .o, $(basename $(ASM_SRCS)))

ASM_ELFS = $(basename $(ASM_SRCS))

ASM_BINS = $(addsuffix .bin, $(basename $(ASM_SRCS)))

ASM_MEMS = $(addsuffix .bin.mem, $(basename $(ASM_SRCS)))


C_SRCS = $(shell find $(C_DIR) -name '*.c')

C_ELFS = $(basename $(C_SRCS))

C_BINS = $(addsuffix .bin, $(basename $(C_SRCS)))

C_MEMS = $(addsuffix .bin.mem, $(basename $(C_SRCS)))

C_DEBUG_ASM = $(addsuffix .bin.s, $(basename $(C_SRCS)))


TARGETS = $(addprefix bin/, $(basename $(FILES)))

PREFIX = riscv32-unknown-elf

CC = $(PREFIX)-gcc

C_FLAGS = -static -nostdlib -O2 -Wl,-T$(LD_SCRIPT) -I c/test_lib

LD_SCRIPT = $(C_DIR)/startup.ld

STARTUP_ASM = $(C_DIR)/startup.s

VERILATOR_EXE = $(VERILATOR_OBJ_DIR)/V$(TOP_SRC_NAME)

VERILATOR_OBJS = $(VERILATOR_OBJ_DIR)/V$(TOP_SRC_NAME).cpp

VERILATOR_SIM_SRCS = $(wildcard $(VERILATOR_DIR)/*.cpp) $(GTEST_DIR)/src/gtest-all.cc

all : $(TARGETS) $(ASM_MEMS) $(C_MEMS) $(VERILATOR_EXE)

bin/% : $(SIM_DIR)/%.v $(SRCS) $(MICROCODE)
	mkdir -p bin
	iverilog -Wall -g2012 -o $@ $<

$(MICROCODE): control_signals.json microcode_descriptor.json
	./generate_mc_tables.py control_signals.json microcode_descriptor.json
	mv $(MICROCODE_FILES) src/

$(ASM_MEMS): $(ASM_BINS)
$(ASM_BINS): $(ASM_ELFS)
$(ASM_ELFS): $(ASM_OBJS)
$(ASM_OBJS): $(ASM_SRCS)

$(C_MEMS): $(C_BINS)
$(C_BINS): $(C_ELFS)
$(C_ELFS): $(C_SRCS) $(LD_SCRIPT) $(STARTUP_ASM)

$(C_DEBUG_ASM): $(C_ELFS)

%.o: %.s
	riscv32-unknown-elf-as -o $@ $<

%: %.o
	riscv32-unknown-elf-ld -o $@ $<

%.bin: %
	riscv32-unknown-elf-objcopy -O binary $< $@

%: %.c
	$(CC) $(C_FLAGS) -Xlinker -Map=$@.map -o $@ $< $(STARTUP_ASM)

%.bin.mem: %.bin
	xxd -g4 -c4 -e $< | awk '{print $$2}' > $@

%.bin.s: %
	riscv32-unknown-elf-objdump -d -S $< > $@

$(VERILATOR_OBJS): $(SRCS)
	verilator --cc $(SRC_DIR)/$(TOP_SRC) --exe $(VERILATOR_SIM_SRCS) $(VERILATOR_FLAGS)

$(VERILATOR_EXE): $(VERILATOR_SIM_SRCS) $(VERILATOR_OBJS)
	cd $(VERILATOR_OBJ_DIR); make -f V$(TOP_SRC_NAME).mk

.PHONY : tests run_tests clean all debug

debug: $(C_DEBUG_ASM)

tests: $(VERILATOR_EXE) $(C_ELFS)

run_tests: tests
	$(VERILATOR_EXE)

clean:
	rm -r bin
	rm -f $(ASM_OBJS) $(ASM_ELFS) $(ASM_BINS) $(ASM_MEMS)
	rm -r $(VERILATOR_OBJ_DIR)

print-% : ; @echo $* = $($*)

