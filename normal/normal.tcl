open_project synthesis_result_normal
set_top divider
add_files normal/divider.cpp
add_files -tb normal/testbench.cpp -cflags "-Wno-unknown-pragmas"
open_solution "solution1"

# Specify the target FPGA
set_part xc7a100tcsg324-1

create_clock -period 10 -name default
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
exit