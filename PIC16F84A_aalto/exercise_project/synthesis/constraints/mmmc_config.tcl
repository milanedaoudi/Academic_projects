##############################################################################
## LIBRARY SETS
##############################################################################
source ./flow/scripts/common_vars.tcl
set setup_corners "slow_vdd1v0"
set hold_corners  "fast_vdd1v2" 


# TODO: program this better
foreach setupc $setup_corners {
  puts "Defining timing files for library set ${setupc}.SS"
  create_library_set \
    -name ${setupc}.SS \
    -timing [subst {
      [glob -nocomplain ./flow/tech/lib/${setup_corners}_basicCells_hvt.lib]
    }]
}

foreach holdc $hold_corners {
  puts "Defining timing files for library set ${holdc}.FF"
  create_library_set \
    -name ${holdc}.FF \
    -timing [subst {
      [glob -nocomplain ./flow/tech/lib/${hold_corners}_basicCells_hvt.lib]
    }]
}

##############################################################################
## ADD OCV SETS
##############################################################################
#foreach librarySet $library_sets {
# puts "Defining timing files for library set $librarySet."
#update_library_set -name $librarySet  \
#-aocv [ glob [get_db lib_dir]/<path to OCV sets>
#}
##############################################################################
## OPERATING CONDITIONS
##############################################################################
#regexp -all {^((.*)p(.*)v)(.*)c}  $setupc tot vtot vreal vdec temp

# Create condition form the filename
foreach setupc $setup_corners {
  puts "Creating operating condition for corner $setupc"
  set tempmax 25
  set tempmin -40
  regexp -all {^(.*)_vdd([0-9]+)v([0-9]+)}  $setupc tot vtot vreal vdec
  puts "$vreal.$vdec"
  puts "$tempmax"
  puts "$tempmin"
  create_opcond \
    -name               $setupc.max \
    -process            1 \
    -voltage            $vreal.$vdec \
    -temperature        "$tempmax"
}

foreach holdc $hold_corners {
  puts "Creating operating condition for corner $holdc"
  set temp 25
  regexp -all {^(.*)_vdd([0-9]+)v([0-9]+)}  $setupc tot vtot vreal vdec
  puts "$vreal.$vdec"
  puts "$temp"
  create_opcond \
    -name               $holdc.min \
    -process            1 \
    -voltage            $vreal.$vdec \
    -temperature        "$tempmin"
}

#
##############################################################################
## TIMING CONDITIONS
##############################################################################
foreach setupc $setup_corners {
  puts "Creating timing condition for corner $setupc"
  create_timing_condition \
    -name               $setupc.setup \
    -library_sets       [list ${setupc}.SS  ] \
    -opcond             $setupc.max
}

foreach holdc $hold_corners {
  puts "Creating timing condition for corner $holdc"
  create_timing_condition \
    -name               $holdc.hold \
    -library_sets       [list ${holdc}.FF  ] \
    -opcond             $holdc.min
}


##############################################################################
## RC CORNERS
##############################################################################
#For this process temperature not necessarily modeled
create_rc_corner \
    -name               cworst_CCworst \
    -temperature        $tempmax \
    -qrc_tech           /prog/cadence/gpdk/g045/lan/flow/t1u1/reference_libs/GPDK045/gsclib045_all_v4.4/gsclib045_tech/qrc/qx/gpdk045.tch 

create_rc_corner \
    -name               rcworst_CCworst \
    -temperature        $tempmax \
    -qrc_tech           /prog/cadence/gpdk/g045/lan/flow/t1u1/reference_libs/GPDK045/gsclib045_all_v4.4/gsclib045_tech/qrc/qx/gpdk045.tch 

create_rc_corner \
    -name               cbest_CCbest \
    -temperature        $tempmin \
    -qrc_tech           /prog/cadence/gpdk/g045/lan/flow/t1u1/reference_libs/GPDK045/gsclib045_all_v4.4/gsclib045_tech/qrc/qx/gpdk045.tch 

create_rc_corner \
    -name               rcbest_CCbest \
    -temperature        $tempmin \
    -qrc_tech           /prog/cadence/gpdk/g045/lan/flow/t1u1/reference_libs/GPDK045/gsclib045_all_v4.4/gsclib045_tech/qrc/qx/gpdk045.tch 

##############################################################################
## DELAY CORNERS
##############################################################################
foreach setupc $setup_corners {
  puts "Defining delay corners for $setupc."
  create_delay_corner \
    -name               ${setupc}.setup_rcworst_CCworst \
    -timing_condition   ${setupc}.setup \
    -rc_corner          rcworst_CCworst
}

foreach setupc $setup_corners {
  puts "Defining delay corners for $setupc."
  create_delay_corner \
    -name               ${setupc}.setup_cworst_CCworst \
    -timing_condition   ${setupc}.setup \
    -rc_corner          cworst_CCworst
}

foreach holdc $hold_corners {
  puts "Defining delay corners for $holdc."
  create_delay_corner \
    -name               ${holdc}.hold_rcbest_CCbest \
    -timing_condition   ${holdc}.hold \
    -rc_corner          rcbest_CCbest
}

foreach holdc $hold_corners {
  puts "Defining delay corners for $holdc."
  create_delay_corner \
    -name               ${holdc}.hold_cbest_CCbest \
    -timing_condition   ${holdc}.hold \
    -rc_corner          cbest_CCbest
}

foreach holdc $hold_corners {
  puts "Defining delay corners for $holdc."
  create_delay_corner \
    -name               ${holdc}.hold_cworst_CCworst \
    -timing_condition   ${holdc}.hold \
    -rc_corner          cworst_CCworst
}

foreach holdc $hold_corners {
  puts "Defining delay corners for $holdc."
  create_delay_corner \
    -name               ${holdc}.hold_rcworst_CCworst \
    -timing_condition   ${holdc}.hold \
    -rc_corner          rcworst_CCworst
}

##############################################################################
## CONSTRAINT MODES
##############################################################################
create_constraint_mode \
    -name               func \
    -sdc_files          $SDCFILE
##############################################################################
## ANALYSIS VIEWS
##############################################################################
foreach setupc $setup_corners {
  puts "Defining analysis views for $setupc."
  create_analysis_view \
    -name               func-${setupc}.setup_cworst_CCworst \
    -constraint_mode    func \
    -delay_corner       ${setupc}.setup_cworst_CCworst \
    -power_mode         aon
}

foreach setupc $setup_corners {
  puts "Defining analysis views for $setupc."
  create_analysis_view \
    -name               func-${setupc}.setup_rcworst_CCworst \
    -constraint_mode    func \
    -delay_corner       ${setupc}.setup_rcworst_CCworst \
    -power_mode         aon
}

foreach holdc $hold_corners {
  puts "Defining analysis views for $holdc."
  create_analysis_view \
    -name               func-${holdc}.hold_cbest_CCbest \
    -constraint_mode    func \
    -delay_corner       ${holdc}.hold_cbest_CCbest \
    -power_mode         aon
}

foreach holdc $hold_corners {
  puts "Defining analysis views for $holdc."
  create_analysis_view \
    -name               func-${holdc}.hold_rcbest_CCbest \
    -constraint_mode    func \
    -delay_corner       ${holdc}.hold_rcbest_CCbest \
    -power_mode         aon
}

foreach holdc $hold_corners {
  puts "Defining analysis views for $holdc."
  create_analysis_view \
    -name               func-${holdc}.hold_cworst_CCworst \
    -constraint_mode    func \
    -delay_corner       ${holdc}.hold_cworst_CCworst \
    -power_mode         aon
}

foreach holdc $hold_corners {
  puts "Defining analysis views for $holdc."
  create_analysis_view \
    -name               func-${holdc}.hold_rcworst_CCworst \
    -constraint_mode    func \
    -delay_corner       ${holdc}.hold_rcworst_CCworst \
    -power_mode         aon
}
##############################################################################
## LIBRARY SETS
##############################################################################
set_analysis_view \
    -setup  [list func-${setupc}.setup_cworst_CCworst ] \
    -hold   [list func-${holdc}.hold_cbest_CCbest ]

