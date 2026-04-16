vlib work

#All the verilog modules
vlog "fp_normalizer.v" 
vlog "fpadd_single.v"
vlog "testbench.v"

vsim -t ps work.testbench
view wave
#add wave *
run -all
