# OSCI Bringup 
Various files for bringing up the EE290C Spring '21 chip

Contents:
* `chip-sw/` - C programs to loaded and executed on test chip 
  * taken from here: https://bwrcrepo.eecs.berkeley.edu/EE290C_EE194_tstech28/ee290c-software-stack.git
* `fesvr/` - scripts and C programs for running FESVR TSI C program on FPGA RISCV Softcore
  * https://bwrcrepo.eecs.berkeley.edu/beagle/beagle-fesvr
  * https://github.com/riscv-software-src/riscv-isa-sim
* `sdc-sw/` - scripts and C programs dealing with mounting and formatting SD Card
  * `images/` - Linux image built using Firemarshal
  * `overlay/` - all files on SD Card file system
  * `marshal-configs/` - taken from here: https://bwrcrepo.eecs.berkeley.edu/beagle/beagle-vcu118-sd-sw.git
* `uart/` - scripts dealing with UART connection between lab machine and FPGA
* `vcu118/`- files specific to VCU118 setup
  * currently only contains latest bitstream on FPGA, generated by Chipyard
  * technically this should include our chipyard fork as a submodule (but that would be too nested): https://github.com/ucberkeley-ee290c/chipyard-osci-bringup.git