# High level synthesis used to generate pipelined fixed pointed divider
## Notes
- Verilog code is in synthesis_result/solution1/syn/verilog/
- Seemed to have fixed issue with incorrect values
- will try AXI interface tomorrow

## Usage
- In home directory
```bash
source /mada/software/Xilinx/Vitis/2023.1/settings64.sh
source /opt/xilinx/xrt/setup.sh
```
- At project root
```bash
make build
make clean
```
