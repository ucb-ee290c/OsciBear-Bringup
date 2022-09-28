# Lightweight FESVR
A lightweight FESVR (frontend server) that works on any Linux and Windows PC backend, designed for bringup chips directly through TSI adapters, without any large FPGA cores in the middle. Much of this support will remain only for TSI initially, but once we can talk to TSI, we can talk to any peripheral MMIO ports by talking to the adapter's physical or virtual TL bus directly. Decision choice remains using C++ since we want high portability and potential backwards compatibility with the original actual FESVR code. We will try to be compatible with the original FESVR, but that infrastructure is so large it will take sometime to sort out how to make the port. The priority remains to be about lightweight bringup of chips.  

## Hardware Implementations
Different hardware implementations require different driver code, and may have different capabilities. Currently we have an TLAdapter on FPGA and microcontroller. 

## UART-TSI Drivers
Uses the TSI Adapter on either FPGA or microcontroller. Current revision only supports FPGA, but will adapt the PyFTDI script written already to support microcontroller (mainly byte and bit alignment differences). All UART drivers will use the UART library https://github.com/FlorianBauer/rs232 at 3M baud, the maximum common baud supported by Windows and Linux. Submodule is already included, please `git submodule update --init --recursive`.

## Ethernet-TSI Drivers
No hardware has been developed yet, but to improve bandwidth to 25-100Mbps down the road we plan to extend support to talk to the FPGA/microcontroller through ethernet. On the reciever, we will run FreeRTOS+TCP stack that supports POSIX style sockets; on the host, we will also write drivers through POSIX socket and port to windows through Cygwin. 