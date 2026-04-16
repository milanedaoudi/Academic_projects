vlib work

#All the verilog modules
vlog "posit_add.v"
vlog "posit_mult.v"
vlog "posit_mac.v"
vlog "mac_testbench.v"

vsim -t ps work.posit_mac_tb
view wave
#add wave *
run -all
