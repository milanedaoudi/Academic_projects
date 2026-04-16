# PROJECT:     spi_slave
# DESCRIPTION: SDC constraints
# AUTHOR:      Configure script
#===============================================================================

set_time_unit -nanoseconds
set_load_unit -picofarads

#----------------------------------
# Mode Definition: Functional mode;
#----------------------------------
#set_case_analysis 0 [get_port io_clkrxvin ]

#--------------------------
# Clock Definitions;
#--------------------------

# Clock Periods
# clock parameters

set COREMODULE spi_slave
set MASTER_CLK_P 10.0 ; 
set SCLK_P 10.0 ; 


# PRIMARY CLOCKS
# This is the master. Core, serial interface tx divider
create_clock -name DSP_MASTER -period $MASTER_CLK_P [get_port "clock" ]

# Spi sclk
create_clock -name SCLK -period $SCLK_P [get_port "io_sclk" ]

report_clocks
report_clocks -generated

# clock groups
set_clock_group -name ASYNC -async -group {DSP_MASTER \
                                       }\
                                   -group {SCLK}

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
# Path Exceptions;
#--------------------------
set_false_path -from  [get_port reset ]
#set_false_path -rise_from [get_db [get_db insts  IOPAD_RESET_0 ] .pins *C ]

set_db timing_io_use_clock_network_latency ideal
#set_db timing_io_use_clock_network_latency always

# latency and uncertainty for Genus
set_clock_latency 1.0 [get_clock DSP_MASTER]
set_clock_latency 1.0 [get_clock SCLK]
set_clock_uncertainty 0.1 [all_clocks]
#deserializer may benefit from multi_cycle_path from 1.2GHz clock to 300MHz cloc

#--------------------------
# I/O Constraints
#--------------------------

# Inputs
set_input_delay [expr 0.1*${SCLK_P}] -clock SCLK [get_ports io_mosi ]
set_input_delay [expr 0.1*${SCLK_P}] -clock SCLK [get_ports io_cs ]
set_input_delay [expr 0.1*${MASTER_CLK_P}] -clock DSP_MASTER [get_ports io_monitor_in* ]
set_input_transition  1.0 [all_inputs]

set_driving_cell -lib_cell INVX4HVT  [all_inputs]



# Outputs
set_output_delay [expr 0.1*${MASTER_CLK_P}] -clock DSP_MASTER  [get_ports io_config_out* ]
set_output_delay [expr 0.1*${SCLK_P}] -clock SCLK  [get_ports io_miso ]
# Is this reasonable
#set_load 2.0 [all_outputs]
set_load 0.015 [all_outputs]
