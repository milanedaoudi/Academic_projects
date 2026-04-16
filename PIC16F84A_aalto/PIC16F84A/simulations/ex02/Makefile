# Makefile for running the GHDL simulation and analyzing the resulting signals with GTKWave

# The path of the VHDL directory relative to the Makefile
VHDL_PATH = ../../vhdl/ex02/

# The VHDL files to compile
FILES = $(VHDL_PATH)multiplexer_2to1.vhd $(VHDL_PATH)tb_ex02.vhd

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
	$(GHDL_CMD) -r $(GHDL_FLAGS) --workdir=$(WORKDIR) $(TESTBENCH) --vcd=$(TESTBENCH).vcd 
	mv $(TESTBENCH).vcd $(WORKDIR)/

view:
	$(WAVEFORM_VIEWER) --dump=$(WORKDIR)/$(TESTBENCH).vcd

clean:
	rm -rf $(WORKDIR)

