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
    // Clock related variables
    reg clk;
    parameter CLOCK_FREQ = 125_000_000;
    parameter CLOCK_PERIOD = 1_000_000_000 / CLOCK_FREQ;
    parameter BAUD_RATE = 115_200;
    parameter BAUD_PERIOD = 1_000_000_000 / BAUD_RATE;
    initial clk = 0;                                        // Clock generation
    always #(CLOCK_PERIOD / 2) clk = ~clk;

    // PC related variables
    reg [7:0] UART_data = 8'd8; initial UART_data = 8'd8;   // 1 byte data from PC to top
    wire PC_Valid; assign PC_Valid = 1'b1;                  // Right now, data from PC is always valid
    reg PC_data_to_UART; initial PC_data_to_UART = 1'b1;    // UART data bits
    // PC related task
    integer i;                                              // Bit index
    task PC_to_UART;
        begin
            PC_data_to_UART = 0;                            // Start bit is 0 to signify start of comm
            #(BAUD_PERIOD);                                 // Wait one buad period before sending data
            for (i = 0; i < 8; i = i + 1) begin             // Send the test data bit-by-bit
                PC_data_to_UART = UART_data[i];             // Set the bit to be the ith bit of the UART data
                #(BAUD_PERIOD);                             // Wait one buad period before sending data
            end
            PC_data_to_UART = 1;                            // End bit is 0 to signify end of comm
            #(BAUD_PERIOD);
        end
    endtask

    // TL related variables
    reg TL_CLK;
    parameter TL_RATE = 1_000_000; 
    parameter TL_PERIOD = 1_000_000_000 / TL_RATE;
    initial TL_CLK = 0;                                     // Clock generation
    always #(TL_PERIOD / 2) TL_CLK = ~TL_CLK;
    wire TL_OUT_VALID, TL_OUT_BITS, TL_OUT_READY;
    reg TL_OUT_READY_reg; 
    assign TL_OUT_READY = TL_OUT_READY_reg; 
    initial TL_OUT_READY_reg = 1'b0;                        // Allows testbench to more easily control when
                                                            // to accept adapter data

    traffic_adapter #(
        .BAUD_RATE(BAUD_RATE)
    ) adapter (
        .sysclk(clk),
        .reset(rst),
        // UART lines
        .uart_rx(PC_data_to_UART),
        .uart_tx(),
        // TileLink Clock signal
        .tl_clk(TL_CLK),
        // FPGA to testchip 
        .tl_out_valid(TL_OUT_VALID),
        .tl_out_ready(1'b1),                                // tb always ready to recieve from the DUT
        .tl_out_bits(TL_OUT_BITS),
        // testchip to FPGA link - not testing in 
        // this module (see tl_recieve_tb.v)
        .tl_in_valid(),
        .tl_in_ready(),
        .tl_in_bits()  
    );

    // Testbench output data from adapter array
    reg [7:0] data_array [255:0];                           // Buffer to hold incoming data
    integer index, size;

    initial begin                                           // Right now, the only thing this testbench is doing 
                                                            // is sending the number 8
        // Hold reset
        rst = 1; repeat (10) @(posedge clk);
        // Delay for some time
        // No data should be transmitted
        rst = 0; repeat (10) @(posedge clk);

        // Send test data to the adapter
        PC_to_UART();                                       // Sending 8 bits of data
        size = UART_data; 
        PC_to_UART();                                       // Sending 8

        repeat (1) @(posedge tl_out_valid);                 // Wait for adapter to have valid data
        repeat (10) @(posedge clk);                         // Nothing should happen

        index = 0; TL_OUT_READY_reg = 1'b1;                 // Readay to get data
        repeat (size) @(posedge TL_CLK) begin               // I'm assuming one bit is pushed out every TL_CLK cycle
            #(TL_PERIOD / 2);                               // Wait until half cycle before sampling
            data_array[0][index] = TL_OUT_BITS;             // Right now, only populating one data point
            index = index + 1;                              // Increment bit, earilest bit input at the lowest index
        end TL_OUT_READY_reg = 1'b0;                        // Data acquired; testbench doesn't request anyone data rn
        
        repeat (5) @(posedge clk);                          // Nothing should happen
        $finish();                                          // Terminate simulation
    end
endmodule