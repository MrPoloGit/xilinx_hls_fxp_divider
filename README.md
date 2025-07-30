# High level synthesis used to generate pipelined fixed pointed divider
## Vitis HLS
- Need to complete testbench, with more tests
- Fix ready valid interface
- need to be able to handle too larger or smaller numbers but hopefully Vitis actually does this under the hood
- will probably need to make things more generic for the divider
- maybe I will add more generic useful modules in here?

## Vivado HLS
- Will be removed as from what I checked the pragma being used are essentially the same as what is being used in Vitis HLS

To Run

```bash
vitis_hls -f script.tcl
```