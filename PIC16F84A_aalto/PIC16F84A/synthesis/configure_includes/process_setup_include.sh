# This file is to be included to configure
TECHROOT="${PDK_HOME}"
FLOWPATH="${ROOTPATH}/flow"
DEFPATH="${ROOTPATH}/def"
LEFPATH="${PDK_HOME}"
CUSTOMLEFPATH=""

CONSTRAINTPATH="${ROOTPATH}/constraints"
SCRIPTPATH="${FLOWPATH}/scripts"
TECHPATH="${FLOWPATH}/tech"
RTLPATH="${ROOTPATH}/source"

#############################################################################
#The process and kit dependent variables
#############################################################################
#Process node definition for some cadence variables 
NODE="45"
TARGETFLAVOR="HVT"  # HVT | RVT | LVT | SLVT
SETUP_CORNERS="slow_vdd1v0"
HOLD_CORNERS="fast_vdd1v2"

#LEF and LIB library file paths
TECHLEF="${PDK_HOME}/gsclib045_tech/lef/gsclib045_tech.lef"

echo $TARGETFLAVOR
#These could be a list of library definitions
#Prefix string for all cells
PREFIX=""
if [ ${TARGETFLAVOR} == "HVT" ]; then
    POSTFIX="HVT"
    TARGETLEFS="${LEFPATH}/gsclib045_hvt/lef/gsclib045_hvt_macro.lef"
    TARGETLIBS="${PDK_HOME}/gsclib045_hvt/timing"
    DRIVING_CELL="INVX4HVT"
elif [ ${TARGETFLAVOR} == "RVT" ]; then
    echo "Screw you! $TARGETFLAVOR not available"
elif [ ${TARGETFLAVOR} == "LVT" ]; then
    POSTFIX="LVT"
    TARGETLEFS="${LEFPATH}/gsclib045_lvt/lef/gsclib045_lvt_macro.lef"
    TARGETLIBS="${PDK_HOME}/gsclib045_lvt/timing"
    DRIVING_CELL="INVX4LVT"
elif [ ${TARGETFLAVOR} == "SLVT" ]; then
    echo "Screw you! $TARGETFLAVOR not available"
else
    echo "Screw you! $TARGETFLAVOR not available"
fi
IOLIBS=""
IOLEFS=""

# QRCFILES
QRCMINRCFILE="${PDK_HOME}/gsclib045_tech/qrc/qx/gpdk045.tch"
QRCMINCFILE="${PDK_HOME}/gsclib045_tech/qrc/qx/gpdk045.tch"
QRCMAXRCFILE="${PDK_HOME}/gsclib045_tech/qrc/qx/gpdk045.tch"
QRCMAXCFILE="${PDK_HOME}/gsclib045_tech/qrc/qx/gpdk045.tch"

# Create directories
for dir in \
    ${ROOTPATH} \
    ${FLOWPATH} \
    ${SCRIPTPATH} \
    ${RTLPATH} \
    ${RTLPATH}/includes \
    ${CONSTRAINTPATH} \
    ${TECHPATH} \
    ${TECHPATH}/def \
    ${TECHPATH}/lef \
    ${TECHPATH}/lib \
    ${TECHPATH}/gds \
    ${TECHPATH}/spice \
    ${MACROPATH}; do
  if [ ! -d "$dir" ]; then
    echo "Creating directory $dir"
    mkdir -p $dir
  fi
done

#############################################################################
# Linking LEF's for analog macros and placeholders
#############################################################################
TARGET=${TECHPATH}/lef
rm -f ${TARGET}/*
echo "Creating symbolic links for LEF's to ${TARGET}"
for linktarg in \
	${TARGETLEFS} \
	${IOLEFS} \
    ${TECHLEF}
do
    ln -sf $linktarg ${TARGET}/
done

##############################################################################
## Linking LIB's
## This is to collect all the possible libs to same dir
##############################################################################
TARGET=${TECHPATH}/lib
rm -f ${TARGET}/*
echo "Creating symbolic links for LIB's to ${TARGET}"
## Add also tech specific stuff here
for lib in ${TARGETLIBS}; do
    for linktarg in \
        "$lib/*.lib"
    do
        ln -sf $linktarg ${TARGET}/
    done
done

# IO libs are not zipped
for lib in ${IOLIBS}; do
    for linktarg in \
        "${LIBPATH}/$lib/*.lib"
    do
        ln -sf $linktarg ${TARGET}/
    done
done

