# High level synthesis used to generate pipelined fixed pointed divider
## Notes
- Verilog code is in synthesis_result/solution1/syn/verilog/
- Seemed to have fixed issue with incorrect values
- normal and axi_1 are working, however axi_1 seems to have pipelining issues
- For some reason axi_2 is failing C post checking, I believe it has to do with data types, saturation

## Usage
- In home directory
```bash
source /mada/software/Xilinx/Vitis/2023.1/settings64.sh
source /opt/xilinx/xrt/setup.sh
```
- At project root
```bash
# Build normal version
make normal

# Build AXI version 1
make axi_1

# Build AXI version 2
make axi_2

# Clean all builds
make clean
```
