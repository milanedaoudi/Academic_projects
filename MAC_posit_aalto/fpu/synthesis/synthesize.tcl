#Variable definitions
source ./flow/scripts/common_vars.tcl
#Technology setups
# mmmc_config also imports timing constraints from SDCFILES
read_mmmc ${CONSTRAINTPATH}/mmmc_config.tcl

#Technology setups
read_physical -lefs  ${TECHPATH}/lef/gsclib045_tech.lef
read_physical -add_lefs "[glob ${TECHPATH}/lef/*_macro.lef]"

#Design files
puts " $TOPMODULE reading RTL"
if {[llength $VERILOGFILES]} {
  puts "Reading in verilog files"
  read_hdl $VERILOGFILES
} else {
  puts "No verilogfiles to read"
}

if {[llength $VHDLFILES]} {
  puts "Reading in vhdl files"
  read_hdl -language vhdl $VHDLFILES
} else {
  puts "No vhdlfiles to read"
}

# Elaborate and initialization
elaborate $TOPMODULE
init_design -top $TOPMODULE

# Power intent
read_power_intent $CPFFILE -cpf
#commit_power_intent

#Timing path groups
#- Clear existing path_groups
get_db cost_groups -if {.name != default} -foreach {delete_obj $object}

#- Add basic path_groups
foreach mode [get_db constraint_modes -if {.is_setup}] {
set_interactive_constraint_mode $mode
group_path -name in2out -from [all_inputs] -to [all_outputs]
if {[sizeof_collection [all_registers]] > 0} {
  group_path -name in2reg -from [all_inputs] -to [all_registers]
  group_path -name reg2out -from [all_registers] -to [all_outputs]
  group_path -name reg2reg -from [all_registers] -to [all_registers]
}
}
set_interactive_constraint_mode {}

#Synthesis commands
syn_generic
write_db ${DBSPATH}/syn_generic.db
syn_map
write_db ${DBSPATH}/syn_map.db
syn_opt
write_db ${DBSPATH}/syn_opt.db

#Timing reports
report_timing -nworst 10 > ${REPORTPATH}/timing.rpt
report_timing_summary -checks {setup drv} > ${REPORTPATH}/setup.analysis_summary.rpt
report_timing_summary -checks {setup drv} -expand_views > ${REPORTPATH}/setup.view_summary.rpt
report_timing_summary -checks {setup drv} -expand_views -expand_clocks launch_capture > ${REPORTPATH}/setup.group_summary.rpt 
report_qor > ${REPORTPATH}/qor.rpt

# Power reports
report_gates -power   > ${REPORTPATH}/gates.rpt
report_clock_gating   > ${REPORTPATH}/clock_gating.rpt
report_power -by_category > ${REPORTPATH}/power.rpt
report_clocks -generated   > ${REPORTPATH}/clocks_generated.rpt
report_area -min_count 1000 > ${REPORTPATH}/area.summary.rpt
report_dp > ${REPORTPATH}/area.datapath.rpt

puts "$TOPMODULE synthesized"

