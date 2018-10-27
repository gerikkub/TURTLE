
SIM_DIR = sim
SRC_DIR = src

ASM_DIR = asm

C_DIR = c

MICROCODE = src/cs_mapper_mod.v src/microcode_mod.v src/metadata_vector.txt opcode_vector.txt subop_vector.txt

SRCS = $(wildcard $(SRC_DIR)/*.v)

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

TARGETS = $(addprefix bin/, $(basename $(FILES)))

PREFIX = riscv32-unknown-elf

CC = $(PREFIX)-gcc

C_FLAGS = -static -nostdlib -O2 -Wl,-T$(LD_SCRIPT)

LD_SCRIPT = $(C_DIR)/startup.ld

STARTUP_ASM = $(C_DIR)/startup.s

all : $(TARGETS) $(ASM_MEMS) $(C_MEMS)

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
$(C_ELFS): $(C_SRCS)

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

clean:
	rm -r bin
	rm -f $(ASM_OBJS) $(ASM_ELFS) $(ASM_BINS) $(ASM_MEMS)

print-% : ; @echo $* = $($*)

