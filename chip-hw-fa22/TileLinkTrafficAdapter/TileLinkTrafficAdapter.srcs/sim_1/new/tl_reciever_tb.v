`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: UC Berkeley
// Engineer: Dang Le
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
*/
module tl_transmitter_tb();
    // Clock parameters
    reg clk;
    parameter CLOCK_FREQ = 125_000_000;
    parameter CLOCK_PERIOD = 1_000_000_000 / CLOCK_FREQ;
    parameter BAUD_RATE = 115_200;
    parameter BAUD_PERIOD = 1_000_000_000 / BAUD_RATE;
    initial clk = 0;
    always #(CLOCK_PERIOD / 2) clk = ~clk;
    wire TL_CLK; reg TL_CLK_reg; assign TL_CLK = TL_CLK_reg;
    parameter TL_RATE = 1_000_000; parameter TL_PERIOD = 1_000_000_000 / TL_RATE;
    initial TL_CLK_reg = 0; always #(TL_PERIOD / 2) TL_CLK_reg = ~TL_CLK_reg;
    // PC related variables
    reg [7:0] TSI_data = 256'd1013; //Test data
    wire PC_Valid, UART_data_to_PC;
    reg TSI_data_to_Adapter;
    assign PC_Valid = 1'b0;
    // PC related task
    integer i, cycle;
    task UART_to_PC;
        begin
            @(negedge UART_data_to_PC);
            #(BAUD_PERIOD);
            for (i = 0; i < 8; i = i + 1) begin // Send the test data bit-by-
                #(BAUD_PERIOD / 2);
                UART_data[i] = UART_data_to_PC;
                #(BAUD_PERIOD / 2);
            end
            #(BAUD_PERIOD);
        end
    endtask
    task TSI_to_Adapter;
        begin
            for (i = 0; i < 256; i = i + 1) begin // Send the test data bit-by-bit
                TSI_data_to_Adapter = TSI_data[i];
                repeat (1) @(posedge TL_CLK);
            end
        end
    endtask
    // UART related variables
    wire UART_Ready, UART_Valid;
    //TL related variables
    wire TL_OUT_VALID;
    wire TL_OUT_READY;
    wire TL_OUT_BITS;
    wire TL_IN_VALID; assign TL_IN_VALID = 1'b1;
    wire TL_IN_READY;
    wire TL_IN_BITS; assign TL_IN_BITS = TSI_data_to_Adapter;
    //FIFO related variables
    reg [255:0] bitstring;
    top traffic_control(
        .CLK_125MHZ_FPGA(clk),
        .BUTTONS(BUTTONS_reg),
        .LEDS(),
        .FPGA_SERIAL_RX(PC_data_to_UART),
        .FPGA_SERIAL_TX(UART_data_to_PC),
        // TileLink
        // Clock signal
        .TL_CLK(TL_CLK),
        // FPGA to testchip link
        .TL_OUT_VALID(TL_OUT_VALID),
        .TL_OUT_READY(TL_OUT_READY),
        .TL_OUT_BITS(TL_OUT_BITS),
        // testchip to FPGA link
        .TL_IN_VALID(TL_IN_VALID),
        .TL_IN_READY(TL_IN_READY),
        .TL_IN_BITS(TL_IN_BITS) // Is this going to TSI, or is it TL_IN_BITS?
    );
    reg [7:0] data_array [255:0]; // Buffer to hold data coming
    integer index, size;
    
    initial begin
        // Hold reset
        BUTTONS_reg = 4'b0001; repeat (10) @(posedge clk);
        // Delay for some time; no data should be transmitted
        BUTTONS_reg = 4'b0000; repeat (10) @(posedge clk);
        repeat (10) @(posedge clk);
        bitstring = 256'd0;
        size = UART_data; // Hardcode right now
        index = 0;
        while (!TL_IN_READY) BUTTONS_reg = BUTTONS_reg;
        TSI_to_Adapter();
        UART_to_PC();
        $finish();
    end

    initial begin
        cycle = 0;
        always @(posedge clk) begin
            cycle = cycle + 1;
            if (cycle > 100000) $finish();
        end
    end
endmodule