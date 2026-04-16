#############################################################################
# This file contains process and tool independent flow configurations
# to be included to configure
#############################################################################
# Directory for databases and reports
DBSPATH="${ROOTPATH}/dbs"
REPORTPATH="${ROOTPATH}/reports"

for dir in $DBSPATH $REPORTPATH; do
    mkdir -p $dir
done

# Timing constraint file
SDCFILE="${CONSTRAINTPATH}/timing_constraints.sdc"
CPFFILE="${CONSTRAINTPATH}/power_domains.cpf"

#############################################################################
# Linking verilogs's
#############################################################################
TARGET=${RTLPATH}
echo "Creating symbolic links for verilog's to ${TARGET}"
find ${TARGET}/ -type l -exec rm -f {} \;
VERILOGFILES=""
for linktarg in $LINKEDVERILOG; do
    echo $linktarg
    ln -sf $linktarg ${TARGET}/
    VERILOGFILES="$VERILOGFILES ${RTLPATH}/$(basename $linktarg)"
done
#############################################################################
# Linking vhdls's
#############################################################################
TARGET=${RTLPATH}
echo "Creating symbolic links for vhdl's to ${TARGET}"
VHDLFILES=""
for linktarg in $LINKEDVHDL; do
    echo $linktarg
    ln -sf $linktarg ${TARGET}/
    VHDLFILES="$VHDLFILES ${RTLPATH}/$(basename $linktarg)"
done

#############################################################################
# Generate common variable definitions for flow tcl files
# Can be included with  source [get_db flow_source_directory]/common_vars.tcl
#############################################################################
CURRENTFILE="${SCRIPTPATH}/common_vars.tcl"
echo "Creating ${CURRENTFILE}"
cat << EOF > ${CURRENTFILE}
set TECHROOT "${TECHROOT}"
set FLOWPATH "${FLOWPATH}"
set DEFPATH "${DEFPATH}"
set LEFPATH "${LEFPATH}"
set CUSTOMLEFPATH "${CUSTOMLEFPATH}"

set CONSTRAINTPATH "${CONSTRAINTPATH}"
set SCRIPTPATH "${SCRIPTPATH}"
set TECHPATH "${TECHPATH}"
set RTLPATH "${RTLPATH}"
set DBSPATH "${DBSPATH}"
set REPORTPATH "${REPORTPATH}"
set TOPMODULE "${TOPMODULE}"
set VERILOGFILES "${VERILOGFILES}"
set VHDLFILES "${VHDLFILES}"
set SDCFILE "${SDCFILE}"
set CPFFILE "${CPFFILE}"

EOF


