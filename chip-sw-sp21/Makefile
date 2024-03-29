#=======================================================================
# EE290C Makefile, adapted from the following
#-----------------------------------------------------------------------
# UCB VLSI FLOW: Makefile for riscv-baseband_tests
# Yunsup Lee (yunsup@cs.berkeley.edu)
#

XLEN ?= 64

default: all

src_dir = .

#--------------------------------------------------------------------
# Sources
#--------------------------------------------------------------------

source_directories = \
	aes \
	baseband \
	aes-baseband \
	core \

core_tests = \
	core/hello \
	core/timer_test \
	core/hellodebug \

baseband_tests = \
	baseband/baseband-basic \
	baseband/baseband-tuning \
	baseband/baseband-debug \
	baseband/baseband-tx \
	baseband/baseband-rx \
	baseband/baseband-lut \

aes_tests = \
	aes/aes-ecb-single-poll \
	aes/aes-ecb-single-int \
	aes/aes-ecb-multiple-poll \
	aes/aes-ecb-consecutive-poll \
    aes/aes-cbc-single-poll \
	aes/aes-ctr-single-poll \

aes_baseband_tests = \
	aes-baseband/aes-baseband-tx \

tests = $(core_tests) $(baseband_tests) $(aes_tests) $(aes_baseband_tests)


#--------------------------------------------------------------------
# Build rules
#--------------------------------------------------------------------

RISCV_PREFIX ?= riscv$(XLEN)-unknown-elf-
RISCV_GCC ?= $(RISCV_PREFIX)gcc
RISCV_GCC_OPTS ?= -DPREALLOCATE=1 -mcmodel=medany -static -std=gnu99 -O2 -ffast-math -fno-common -fno-builtin-printf -fno-tree-loop-distribute-patterns -march=rv32imafc -mabi=ilp32f -mno-fdiv
RISCV_LINK ?= $(RISCV_GCC) -T $(src_dir)/common/test.ld $(incs)
RISCV_LINK_OPTS ?= -static -nostdlib -nostartfiles -lm -lgcc -T $(src_dir)/common/test.ld -march=rv32imafc -mabi=ilp32f
RISCV_OBJDUMP ?= $(RISCV_PREFIX)objdump --disassemble-all --disassemble-zeroes --section=.text --section=.text.startup --section=.text.init --section=.data
RISCV_STRIP = $(RISCV_PREFIX)strip
RISCV_SIM ?= spike --isa=rv$(XLEN)gc

incs  += -I$(src_dir)/env -I$(src_dir)/common $(addprefix -I$(src_dir)/, $(source_directories))
objs  :=

#------------------------------------------------------------
# Build and run benchmarks on riscv simulator

tests_riscv_bin  = $(addsuffix .riscv,  $(tests))
tests_riscv_debug_bin  = $(addsuffix .debug.riscv,  $(tests))
tests_riscv_dump = $(addsuffix .riscv.dump, $(tests))
tests_riscv_out  = $(addsuffix .riscv.out,  $(tests))

%.debug.riscv: %.c $(wildcard $(src_dir)/common/*.c) $(wildcard $(src_dir)/common/*.S)
	$(RISCV_GCC) $(incs) $(RISCV_GCC_OPTS) -o $@ $^ $(RISCV_LINK_OPTS)

$(tests_riscv_bin): %.riscv: %.debug.riscv
	$(RISCV_STRIP) $< -g -o $@

$(tests_riscv_dump): %.riscv.dump: %.debug.riscv
	$(RISCV_OBJDUMP) $< > $@

$(tests_riscv_out): %.riscv.out: %.debug.riscv
	$(RISCV_SIM) $< > $@

riscv: $(tests_riscv_dump) $(tests_riscv_bin)
run: $(tests_riscv_out)

junk += $(tests_riscv_bin) $(tests_riscv_debug_bin) $(tests_riscv_dump) $(tests_riscv_hex) $(tests_riscv_out)

#------------------------------------------------------------
# Default

all: riscv

#------------------------------------------------------------
# Clean up
print:
	@echo $(tests)

clean:
	rm -rf $(objs) $(junk)
