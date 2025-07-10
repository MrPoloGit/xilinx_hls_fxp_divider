open_project synthesis_result
set_top divider
add_files divider.cpp
add_files -tb testbench.cpp -cflags "-Wno-unknown-pragmas"
open_solution "solution1"

# Specify the target FPGA
set_part xc7z020clg400-1

create_clock -period 10 -name default
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
exit
