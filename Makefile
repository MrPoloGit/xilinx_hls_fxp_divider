PROJECT_DIR := fixed_point_divider
SCRIPT      := script.tcl
LOG         := vitis_hls.log

.PHONY: all build clean

all: build

build:
	vitis_hls -f $(SCRIPT) | tee $(LOG)

clean:
	rm -rf $(PROJECT_DIR) $(LOG)
