vlib work

#All the verilog modules
vlog "fmultiplier.v" 
vlog "fmultiplier_tb.v"

vsim -t ps work.fmultiplier_tb
view wave
#add wave *
run -all
