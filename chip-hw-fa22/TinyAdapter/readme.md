# Uart-TSI Converter
A simple serial to serial converter, from uart packets to TSI. [Under construction, debug needed]. Bugs:
 1. General: shiftReg shifts increments after detecting TSI rising clk, need to make it count an extra cycle or TSI might not catch the first bit. (Make it latch?)
 2. TX: shifts out during uart's stop bit. If TSI is fast enough, it might trigger some unknown bug, for now it is safe because inputs won't begin to log unless uart line is on a falling edge (start bit), and it won't be detected on a rising edge (stop bit).
 3. RX: working, no bugs from first look.

## Packet Format
There are no headers or frame preambles, just consecutive completely serial TSI packets rounded up to the nearest byte so uart can transmit. Both transmits LSBit first. 

## Reset Mechanism
When all the TSI bits are high, reset the testchip first for 0.5s, then reset itself for 0.5s. The computer should send exactly two TSI packets worth of 1s to guarentee reset when FPGA is in an unknown state. No mechanism of informing computer waking up upon reset currently for simplicity, because host can just wait for >1s after sending exactly 2x TSI all highs to guarentee FPGA is in a stable state.  

Edge case: if the memory bus is frozen, say TL_OUT_RD is low for a long time, nothing wil be able to reset if shiftReg is waiting for that to clear. To clear this edge case, there must be a separate detector that detects the amount of continuous all high UART packets to trigger the reset regardless of the state of the TL_OUT. For now this case is ignored, but it may happen. 

## Timeout Mechanism
No hardware timeouts are present for sake of simplicity. Implementing this in software. Should host detect no response after a certain TSI query, they should timeout and reset everything. 