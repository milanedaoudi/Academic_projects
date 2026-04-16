# PROJECT:     fpadd_single
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
set COREMODULE fpadd_single
set MASTER_CLK_P 2.5

# PRIMARY CLOCKS
# Core clock for fpadd_single module
create_clock -name DSP_MASTER -period $MASTER_CLK_P [get_port "clk"]

report_clocks
report_clocks -generated

# Clock groups
set_clock_group -name ASYNC -async -group {DSP_MASTER}

#--------------------------
# Path Exceptions
#--------------------------
set_false_path -from [get_port reset]

set_db timing_io_use_clock_network_latency ideal
set_clock_latency 1.0 [get_clock DSP_MASTER]
set_clock_uncertainty 0.1 [all_clocks]

#--------------------------
# I/O Constraints
#--------------------------

# Inputs
set_input_delay [expr 0.1 * ${MASTER_CLK_P}] -clock DSP_MASTER [get_ports {
    reset
    reg_A
    reg_B
}]
set_input_transition 1.0 [all_inputs]
set_driving_cell -lib_cell INVX4HVT [all_inputs]

# Outputs
set_output_delay [expr 0.1 * ${MASTER_CLK_P}] -clock DSP_MASTER [get_ports {
    out
}]
set_load 0.015 [all_outputs]

