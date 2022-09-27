# FESVR Backend
A unified FESVR (frontend server) port that should work on any Linux and windows PC backend, or an RISC-V core as written originally for FESVR. Much of support will remain only for TSI initially, but once we can talk to TSI, we can talk to GPIO ports by talking to the TSI bus directly. Decision choice remains using C++ since we want high portability and backwards compatibility with the original FESVR code. 

## Hardware Implementations
Different hardware implementations require different driver code, and may have different capabilities. 

## UART-TSI Drivers
Uses the TSI Adapter on either FPGA or microcontroller. Current revision only supports FPGA, but will adapt the PyFTDI script written already to support microcontroller (mainly byte and bit alignment differences). All UART drivers will use the UART library https://github.com/FlorianBauer/rs232 at 3M baud, the maximum common baud supported by Windows and Linux. 

## Ethernet-TSI Drivers
No hardware has been developed yet, but to improve bandwidth to 25-100Mbps down the road we plan to extend support to talk to the FPGA/microcontroller through ethernet. On the reciever, we will run FreeRTOS+TCP stack that supports POSIX style sockets; on the host, we will also write drivers through POSIX socket and port to windows through Cygwin. 