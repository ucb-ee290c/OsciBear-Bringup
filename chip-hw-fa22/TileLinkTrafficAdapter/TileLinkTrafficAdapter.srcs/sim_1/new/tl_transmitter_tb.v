`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: UC Berkeley
// Engineer: Dang Le, Thomas Matthew
//
// Create Date: 09/17/2022 11:44:34 PM
// Module Name: tl_transmitter_tb
//
// Dependencies: traffic_adapter.v
//
// Revision:    9/21 7:08pm Removed UART and FIFO wirings and replaced them with the adapter module - DL
//              9/22 5:30pm Cleaned up logic and wirings - TM
//              9/23 5:02pm Reorganized testbench, removed unnecessary wires and regs, and documented more thoroughly - DL
//              9/28 Replace PC_UART task with built in UART module - TM
//              9/29 Work with Daniel clarifying adapter protocol and clean up testbench design. Add packet size param - TM
// Additional Comments: This module tests the transmit (PC(UART)->chip(TSI)) side of the traffic_adapter
//
//////////////////////////////////////////////////////////////////////////////////

/*
TO TEST:
UART BAUD faster than TL TSI out rate
Chipside TL ready goes low and see that TSI waits
Determine how bits are being morphed as they are sent over so software can code accordingly
*/
module tl_transmitter_tb();

    parameter TL_PACKET_SIZE = 123;
    parameter BYTES_TO_SEND = (TL_PACKET_SIZE+8-1)/8; // Rounds up # bytes

    parameter CLOCK_FREQ = 125_000_000;
    parameter CLOCK_PERIOD = 1_000_000_000 / CLOCK_FREQ;
    parameter BAUD_RATE = 1_000_000;
    parameter BAUD_PERIOD = 1_000_000_000 / BAUD_RATE;
    parameter TL_RATE = 1_000_000; 
    parameter TL_PERIOD = 1_000_000_000 / TL_RATE;
    
    // Clocks (system and TileLink)
    reg TL_CLK;
    reg clk;    
    
    initial TL_CLK = 0;
    always #(TL_PERIOD / 2) TL_CLK = ~TL_CLK;
    
    initial clk = 0;
    always #(CLOCK_PERIOD / 2) clk = ~clk;

    // PC wires
    reg rst;
    reg [7:0] PC_UART_data_arr [16:0];
    reg [7:0] PC_UART_data;
    reg PC_UART_data_valid;
    wire PC_UART_data_ready;
    wire PC_UART_data_to_DUT; 

    // TL WIRES
    wire TL_OUT_VALID;
    wire TL_OUT_BITS;
    wire TL_TX_DONE;

    // TB Wires
    integer i;
    integer send = 1;
    integer recieved = 0;
    reg [122:0] recieved_over_TSI;

    // PC side UART (only use TX side)
    uart #(.CLOCK_FREQ(CLOCK_FREQ), .BAUD_RATE(BAUD_RATE)) PC_UART (
        .clk(clk),
        .reset(rst),

        .data_in(PC_UART_data),
        .data_in_valid(PC_UART_data_valid),
        .data_in_ready(PC_UART_data_ready),

        .serial_out(PC_UART_data_to_DUT)
    );

    // Instatiate DUT
    traffic_adapter #(
        .BAUD_RATE(BAUD_RATE)
    ) adapter (
        .sysclk(clk),
        .reset(rst),
        // UART lines
        .uart_rx(PC_UART_data_to_DUT),
        .uart_tx(),
        // TileLink Clock signal
        .tl_clk(TL_CLK),
        // FPGA to testchip 
        .tl_out_valid(TL_OUT_VALID),
        .tl_out_ready(1'b1),        // tb always ready to recieve from the DUT
        .tl_out_bits(TL_OUT_BITS),
        // testchip to FPGA link    // not testing in this module (see tl_reciever_tb.v)
        .tl_in_valid(),
        .tl_in_ready(),
        .tl_in_bits(),
        // Status Bits
        .tl_tx_done(TL_TX_DONE),
        .tl_rx_busy(),
        .tl_rx_done()
    );
    
    // Recieve Adapter Response
    always @(posedge TL_CLK) begin
        if (TL_OUT_VALID) begin
            recieved_over_TSI[recieved] = TL_OUT_BITS;
            recieved = recieved + 1;
        end
    end

    initial begin 
        // Reset
        rst = 1;
        PC_UART_data_valid = 0;
        // Initialize Arrays:
        PC_UART_data_arr[0] = TL_PACKET_SIZE;       // First byte sent to the adapter is the packet size
        for (i = 1; i < BYTES_TO_SEND + 1; i = i + 1) begin
            PC_UART_data_arr[i] = 8'hf0 + i;            // Send some random data
        end
        repeat (10) @(posedge clk);
        rst = 0; 
        repeat (10) @(posedge clk);
        
        // Send first byte which indicates how many more bytes will be sent
        PC_UART_data = PC_UART_data_arr[0];
        PC_UART_data_valid = 1;
        repeat (1) @(posedge clk);
        PC_UART_data_valid = 0;

        // Send rest of the bytes
        repeat (BYTES_TO_SEND) @(posedge PC_UART_data_ready) begin
            repeat (1) @(posedge clk);
            PC_UART_data = PC_UART_data_arr[send];
            PC_UART_data_valid = 1;
            repeat (1) @(posedge clk);
            PC_UART_data_valid = 0;
            send = send + 1;
        end

        //repeat (1) @(posedge TL_TX_DONE);   // Wait until traffic adapter asserts done
        repeat (50) @(posedge clk);
        $finish();
    end
endmodule

/*
Old problems: (Before meeting w Daniel)
rx_fifo not properly reading data in. getting xxx because reading 1 cycle too soon
this is because rd_en is always on after the first byte transferred, the first byte works but all
subsequent get the value in the rx_fifo buffer 1 cycle before the data is placed there
Proposed FIX: possible to buffer wr_en signal? prob not, will need to make rd_en more granular than
just always on when PARSING


tl_tx_valid bouncing while tl_transmitter in transmit state

edge detector in tl_transmitter not working - its working just not showing on waveform???!

tx_buffered_length only width 1 but 8 bit values passed to it. assuming should have been width 8

change tl_tx_data from assign statment to putting into ff so value persists after fifo value changes

--- 
after meeting:

took out tx_busy state because it was unused
*/