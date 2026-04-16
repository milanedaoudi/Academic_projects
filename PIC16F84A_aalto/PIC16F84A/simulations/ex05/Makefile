# Makefile for running the GHDL simulation and analyzing the resulting signals with GTKWave

# The path of the VHDL directory relative to the Makefile
VHDL_PATH = ../../vhdl/ex05/

# The VHDL files to compile
FILES = $(VHDL_PATH)ALU_Types.vhd $(VHDL_PATH)full_adder.vhd $(VHDL_PATH)Nbit_ripple_carry_adder.vhd $(VHDL_PATH)alu.vhd $(VHDL_PATH)memory.vhd $(VHDL_PATH)tb_ex05.vhd

# Testbench parameters
WORKDIR = work

# GHDL configuration
GHDL_CMD  = ghdl
GHDL_FLAGS = --std=08 --workdir=$(WORKDIR)

# Wave viewer configuration
WAVEFORM_VIEWER = gtkwave

all: clean make run view

make:
ifeq ($(strip $(TESTBENCH)),)
	echo "TESTBENCH missing. Call with TESTBENCH=<tb_entity_name>."
	exit 1
endif

	mkdir -p $(WORKDIR)
	$(GHDL_CMD) -a $(GHDL_FLAGS) $(FILES)
	$(GHDL_CMD) -e $(GHDL_FLAGS) $(TESTBENCH)

run:
	$(GHDL_CMD) -r $(GHDL_FLAGS) --workdir=$(WORKDIR) $(TESTBENCH) --fst=$(TESTBENCH).fst 
	mv $(TESTBENCH).fst $(WORKDIR)/

view:
	$(WAVEFORM_VIEWER) --dump=$(WORKDIR)/$(TESTBENCH).fst

clean:
	rm -rf $(WORKDIR)

