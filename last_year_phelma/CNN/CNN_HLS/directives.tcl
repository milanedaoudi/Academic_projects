solution new -state initial
solution options defaults

options set Project/ProjectNamePrefix HLS
solution options set /Input/TargetPlatform x86_64
solution options set /Input/SearchPath {/usr/include/x86_64-linux-gnu }
solution options set /Input/LibPaths {/usr/lib /usr/lib/x86_64-linux-gnu}
solution options set /Output/GenerateCycleNetlist false
solution options set /TextEditor/FontSize 9
solution options set /Flows/Precision/addio false

flow package option set /QuestaSIM/SCCOM_OPTS {-O2 -g0 -x c++ -Wall -Wno-unused-label -Wno-unknown-pragmas}
flow package require /SCVerify

# -----------------------
# Sources
# -----------------------
solution file add ./include/fixed_var.hpp -type CHEADER
solution file add ./include/coeff.hpp -type CHEADER
solution file add ./include/cnn_hls.hpp -type CHEADER
solution file add ./include/forward_debug.hpp -type CHEADER
solution file add ./include/images.hpp -type CHEADER

solution file add ./images.cpp -type C++
solution file add ./coeff.cpp -type C++
solution file add ./forward_debug.cpp -type C++

solution file add ./TOP_BENCH.cpp -type C++ -exclude true

# -----------------------
# Global directives
# -----------------------
directive set -DA_DISABLE_RESIZE_MEM true

go analyze

solution design set forward_debug_hls -top
go compile

solution library add mgc_Xilinx-ZYNQ-2_beh -- \
  -rtlsyntool Precision \
  -manufacturer Xilinx \
  -family ZYNQ \
  -speed -2 \
  -part xc7z020clg400-2

solution library add Xilinx_RAMS
go libraries

directive set -CLOCKS {clk {
  -CLOCK_PERIOD 20
  -CLOCK_EDGE rising
  -CLOCK_HIGH_TIME 10
  -CLOCK_OFFSET 0.0
  -CLOCK_UNCERTAINTY 0.0
  -RESET_KIND sync
  -RESET_SYNC_NAME rst
  -RESET_SYNC_ACTIVE high
}}

# -----------------------
# INTERFACES
# -----------------------
directive set /forward_debug_hls/img_sel:rsc     -MAP_TO_MODULE {[DirectInput]}
directive set /forward_debug_hls/pred_out:rsc    -MAP_TO_MODULE {[DirectOutput]}
directive set /forward_debug_hls/pred_valid:rsc  -MAP_TO_MODULE {[DirectOutput]}

go assembly

# -----------------------
# PIPELINING
# -----------------------
directive set /forward_debug_hls/core/conv2d_hls<24,24,3,3,64,1,true>:for#2:for:for:for:for -PIPELINE_INIT_INTERVAL 1
directive set /forward_debug_hls/core/maxpool_hls<24,24,64,3,2,true>:for#2:for:for:for -PIPELINE_INIT_INTERVAL 1
directive set /forward_debug_hls/core/conv2d_hls<12,12,64,3,32,1,true>:for#2:for:for:for:for -PIPELINE_INIT_INTERVAL 1
directive set /forward_debug_hls/core/maxpool_hls<12,12,32,3,2,true>:for#2:for:for:for -PIPELINE_INIT_INTERVAL 1
directive set /forward_debug_hls/core/conv2d_hls<6,6,32,3,20,1,true>:for#2:for:for:for:for -PIPELINE_INIT_INTERVAL 1
directive set /forward_debug_hls/core/maxpool_hls<6,6,20,3,2,true>:for#2:for:for:for -PIPELINE_INIT_INTERVAL 1
directive set /forward_debug_hls/core/dense_hls<180,10>:for:for -PIPELINE_INIT_INTERVAL 1

go architect
go allocate
go extract

