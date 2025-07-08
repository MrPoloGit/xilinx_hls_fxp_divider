PROJECT_DIR := fixed_point_divider
SCRIPT      := script.tcl
LOG         := vitis_hls.log
RTL_SRC     := $(PROJECT_DIR)/solution1/syn/verilog
RTL_DST     := rtl

.PHONY: all build clean

all: build

build:
	vitis_hls -f $(SCRIPT) | tee $(LOG)
	mkdir -p $(RTL_DST)
	cp $(RTL_SRC)/*.v $(RTL_DST)/

clean:
	rm -rf $(PROJECT_DIR) $(LOG) $(RTL_DST)
