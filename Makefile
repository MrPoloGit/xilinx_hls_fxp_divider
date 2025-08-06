.PHONY: normal axi_1 axi_2 clean

normal:
	vitis_hls -f normal/normal.tcl | tee vitis_hls_normal.log

axi_1:
	vitis_hls -f axi_1/axi_1.tcl | tee vitis_hls_axi_1.log

axi_2:
	vitis_hls -f axi_2/axi_2.tcl | tee vitis_hls_axi_2.log

clean:
	rm -rf synthesis_result_normal synthesis_result_axi_1 synthesis_result_axi_2 vitis_hls.log vitis_hls_normal.log vitis_hls_axi_1.log vitis_hls_axi_2.log
