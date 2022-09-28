`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: UC Berkeley
// Engineer: Dang Le, Thomas Matthew
//
// Create Date: 09/17/2022 11:44:34 PM
// Design Name:
// Module Name: tl_transmitter_tb
// Project Name:
// Target Devices:
// Tool Versions:
// Description:
//
// Dependencies:
//
// Revision: 7:08Removed UART and FIFO wirings and replaced them with the top module. No need
// to rewire
// Revision 0.01 - File Created
// Additional Comments:
//
//////////////////////////////////////////////////////////////////////////////////
/*
Thomas Notes:
- Might be a good idea to use UART module instead of manually implementing UART in task PC_to_UART
- only need to test traffic_adapter module rather than top module (see inside top.v where traffic_adapter is instatiated)
    - can make things cleaner/simpler
- ran through vivado and fixed some minor syntax errors (mispelling, extra comma, etc)
- changed bitstring and FIFO_data_out widths from 128 to 256 to let it compile (fifo was instantiated to 256 width so has to match)
- it now compiles but I haven't checked waveform for functionality. not entirely sure what you want to happen around line 140 either
*/
module tl_transmitter_tb();
    reg rst;
    // Clock parameters
    reg clk;
    parameter CLOCK_FREQ = 125_000_000;
    parameter CLOCK_PERIOD = 1_000_000_000 / CLOCK_FREQ;
    parameter BAUD_RATE = 115_200;
    parameter BAUD_PERIOD = 1_000_000_000 / BAUD_RATE;
    initial clk = 0;
    always #(CLOCK_PERIOD / 2) clk = ~clk;
    // PC related variables
    reg [7:0] UART_data = 8'd8; //Test data
    wire PC_Valid;
    reg PC_data_to_UART; initial PC_data_to_UART = 1'b1;
    assign PC_Valid = 1'b1;
    // PC related task
    integer i;
    task PC_to_UART;
        begin
            PC_data_to_UART = 0; // Start bit is 0
            #(BAUD_PERIOD);
            for (i = 0; i < 8; i = i + 1) begin // Send the test data bit-by-bit
                PC_data_to_UART = UART_data[i];
                #(BAUD_PERIOD);
            end
            PC_data_to_UART = 1; // End bit is 0
            #(BAUD_PERIOD);
        end
    endtask
    //TL related variables
    reg TL_CLK;
    parameter TL_RATE = 1_000_000; parameter TL_PERIOD = 1_000_000_000 / TL_RATE;
    initial TL_CLK = 0; always #(TL_PERIOD / 2) TL_CLK = ~TL_CLK;
    wire TL_OUT_VALID, TL_OUT_BITS;
    //FIFO related variables
    traffic_adapter #(
        .BAUD_RATE(115_200)
    ) adapter (
        .sysclk(clk),
        .reset(rst),
        /// UART lines
        .uart_rx(PC_data_to_UART),
        .uart_tx(),
        /// TileLink Clock signal
        .tl_clk(TL_CLK),
        // FPGA to testchip 
        .tl_out_valid(TL_OUT_VALID),
        .tl_out_ready(1'b1), // tb always ready to recieve from the DUT
        .tl_out_bits(TL_OUT_BITS),
        // testchip to FPGA link - not testing in this module (see tl_recieve_tb.v)
        .tl_in_valid(),
        .tl_in_ready(),
        .tl_in_bits()
    );
    reg [255:0] data_array [7:0]; // Buffer to hold data coming
    integer index = 1;
    initial begin
        // Hold reset
        rst = 1; repeat (10) @(posedge clk);
        for (i = 0; i < 256; i = i + 1) begin
            data_array[i] = 0;
        end
        // Delay for some time; no data should be transmitted
        rst = 0; repeat (10) @(posedge clk);
        // Send test data to the adapter
        PC_to_UART();
        // Nothing should happen since I didn't allow the FIFO to get the data yet
        repeat (10) @(posedge clk);
        // Initially the bitstream length I want to send has a length of 8. Also, I'm not sure where the bitstring data is coming from
        // For now I'll just assume that there is a byte of data in the adapter_controller waiting to get shifted out
        PC_to_UART();
        repeat (100) @(posedge TL_CLK);
        /*repeat (UART_data) @(posedge TL_CLK) begin
            #(TL_PERIOD / 2); // Wait until half cycle before sampling
            data_array[0][index] = TL_OUT_BITS;
            index = index + 1;
        end*/
        $finish();
    end
endmodule