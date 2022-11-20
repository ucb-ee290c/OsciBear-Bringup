vlib work
vlib riviera

vlib riviera/xpm
vlib riviera/xil_defaultlib

vmap xpm riviera/xpm
vmap xil_defaultlib riviera/xil_defaultlib

vlog -work xpm  -sv2k12 "+incdir+../../../../tsi_arty100.gen/sources_1/ip/ila_2_1/hdl/verilog" \
"C:/Xilinx/Vivado/2022.2/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv" \
"C:/Xilinx/Vivado/2022.2/data/ip/xpm/xpm_memory/hdl/xpm_memory.sv" \

vcom -work xpm -93  \
"C:/Xilinx/Vivado/2022.2/data/ip/xpm/xpm_VCOMP.vhd" \

vlog -work xil_defaultlib  -v2k5 "+incdir+../../../../tsi_arty100.gen/sources_1/ip/ila_2_1/hdl/verilog" \
"../../../../tsi_arty100.gen/sources_1/ip/ila_2_1/sim/ila_2.v" \

vlog -work xil_defaultlib \
"glbl.v"

