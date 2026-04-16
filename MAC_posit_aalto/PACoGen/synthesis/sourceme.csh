#Set the temporary directory to be LARGE and CLUSTERWID
#set UNAME=`whoami`
#set TEMPORARY="/work/$UNAME"
#
#if ( ! -d "${TEMPORARY}"  ) then
#    echo "${TEMPORARY} does not exist"
#   if ( ! $?TEMPDIR ) then
#      echo "TMPDIR is not set"
#   endif
#   echo "We need to use ${TEMPORARY} as a TMPDIR"
#   echo "Otherwise LSF jobs may fail"
#   echo "Create that by mkdir ${TEMPORARY} and re-run this script."
#   exit 
#else
#   if ( -d "${TEMPORARY}"  ) then
#       echo "${TEMPORARY} exists. GOOD"
#   endif
#   if ( ! $?TMPDIR ) then
#      echo "TMPDIR is not set"
#      echo "Defining TMPDIR to be ${TEMPORARY}"
#      echo "Undefine with 'unsetenv TMPDIR' if you do not agree"
#      setenv TMPDIR ${TEMPORARY}
#  endif
#endif

use genus_21.1
use ssv_21.1
use innovus_21.1
use ext_211

setenv PDK_HOME "/prog/cadence/gpdk/g045/lan/flow/t1u1/reference_libs/GPDK045/gsclib045_all_v4.4"


