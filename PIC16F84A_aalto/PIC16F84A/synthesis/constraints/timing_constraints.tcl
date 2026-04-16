# PROJECT:     pic_rtl
# DESCRIPTION: SDC constraints
# AUTHOR:      Configure script
#===============================================================================

set_time_unit -nanoseconds
set_load_unit -picofarads

#----------------------------------
# Mode Definition: Functional mode
#----------------------------------
#set_case_analysis 0 [get_port io_clkrxvin ]

#--------------------------
# Clock Definitions
#--------------------------

# Clock Periods
# clock parameters

set COREMODULE pic_rtl
set MASTER_CLK_P 250.0



# PRIMARY CLOCKS
# This is the master. Core // I changed port clock to clk
create_clock -name DSP_MASTER -period $MASTER_CLK_P [get_port "clk" ]




report_clocks
report_clocks -generated

# clock groups
set_clock_group -name ASYNC -async -group {DSP_MASTER }
                                       
                                   

# You MUST be certain that there are no important paths between these
# groups. This is to disable timing with mutually exclusive cases
# of clock multiplexers. Probably can be defined with higher accuracy
# by defining clk Mux output as a clock source with different group names.

#set_clock_groups -name LOG_EXCL_RX_CLOCK -logically_exclusive \
#                                   -group {
#                                           clkpntx \
#                                           clkp2ntx \
#                                           clkp4ntx \
#                                           clkp8ntx \
#                                       } \
#
#set_clock_groups -name LOG_EXCL_CLOCK -logically_exclusive \
#                                   -group {rxmux_slow} \
#                                   -group {DSP_MASTER } \

#--------------------------
# Path Exceptions
#--------------------------
set_false_path -from  [get_port reset ]
#set_false_path -rise_from [get_db [get_db insts  IOPAD_RESET_0 ] .pins *C ]

set_db timing_io_use_clock_network_latency ideal
#set_db timing_io_use_clock_network_latency always

# latency and uncertainty for Genus
set_clock_latency 1.0 [get_clock DSP_MASTER]

set_clock_uncertainty 0.1 [all_clocks]
#deserializer may benefit from multi_cycle_path from 1.2GHz clock to 300MHz cloc

#--------------------------
# I/O Constraints
#--------------------------

# Inputs
set_input_delay [expr 0.1*${MASTER_CLK_P}] -clock DSP_MASTER [get_ports {
    reset
    instruction
    pc_lath
}]
set_input_transition  1.0 [all_inputs]
set_driving_cell -lib_cell INVX4HVT  [all_inputs]


# Outputs
set_output_delay [expr 0.1*${MASTER_CLK_P}] -clock DSP_MASTER [get_ports {
    pc
    w_register
    alu_A
    alu_B
    alu_result
    alu_op
    alu_bit_select
    alu_status_in
    alu_status_out
    mem_write_addr
    mem_read_addr
    mem_data_in
    mem_data_out
    mem_write_en
    mem_read_en
    status_write_enable
    push
    pop
    stack_data_in
    stack_data_out
}]






# Is this reasonable
#set_load 2.0 [all_outputs]
set_load 0.015 [all_outputs]

