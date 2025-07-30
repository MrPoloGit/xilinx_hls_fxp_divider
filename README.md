# High level synthesis used to generate pipelined fixed pointed divider
## Vitis HLS
- Need to complete testbench, with more tests
- Bug with to be able to handle too larger or smaller numbers
- Bug with Ready-Valid Interface
- Will probably need to make things more generic for the divider, i.e. the fixed point format?
- Need to try and figure out how to make sure that VHDL isn't generated
- Maybe use AXI Ready_Valid Interface?

## Usage
```bash
vitis_hls -f script.tcl
```

- Verilog code is in synthesis_result/solution1/syn/verilog/