PROJECT_DIR := synthesis_result
SCRIPT      := script.tcl
LOG         := vitis_hls.log

.PHONY: build clean

build:
	vitis_hls -f $(SCRIPT) | tee $(LOG)

clean:
	rm -rf $(PROJECT_DIR) $(LOG)
