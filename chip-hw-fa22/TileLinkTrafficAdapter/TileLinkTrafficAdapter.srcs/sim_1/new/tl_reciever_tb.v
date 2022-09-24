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
// Revision:    7/22 5:30pm Wrote skeleton of tl_reciever_tb - DL
//              7/23 5:30pm Cleaned up reciever testbench and reorganized code - DL
// Revision 0.01 - File Created
// Additional Comments:
//
//////////////////////////////////////////////////////////////////////////////////
/*
*/
module tl_reciever_tb();
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
    reg [255:0] TSI_data = 256'd1013;                       // 256 bit data from TSI to adapter
    wire PC_Valid; assign PC_Valid = 1'b0;                  // The PC should never send data to adapter in this 
                                                            // testbench
    reg TSI_data_to_Adapter;                                // TSI data bit
    reg [7:0] UART_data;                                    // Right now, only store one byte from adapter  
    wire UART_data_to_PC;                                   // UART data bit  
    wire UART_Ready, UART_Valid;                            
    assign UART_Valid = 1'b0;                               // This testbench never sends data from PC to adapter  
    reg UART_Ready_reg; assign UART_Ready = UART_Ready_reg; // Allow testbench to control when to read data from adapter                                    
    // PC related tasks
    integer i, cycle;
    task UART_to_PC;                                        // Get data sent from adapter to PC
        begin
            @(negedge UART_data_to_PC);                     // Wait for adapter data to start comm
            #(BAUD_PERIOD);                                 // Wait for first data bit
            for (i = 0; i < 8; i = i + 1) begin             // Measure the data bit-by-bit
                #(BAUD_PERIOD / 2);                         // Measure data in halfway before new data is sent
                UART_data[i] = UART_data_to_PC;             // Set the ith bit to the input bit
                #(BAUD_PERIOD / 2);                         // Wait another half baud period for new data      
            end
        end
    endtask
    task TSI_to_Adapter;                                    // Send TSI data to adapater
        begin
            for (i = 0; i < 256; i = i + 1) begin           // Send the test data bit-by-bit,
                                                            // currently assuming that it's fine to send 256 bits at once
                TSI_data_to_Adapter = TSI_data[i];          // Output the ith TSI bit
                repeat (1) @(posedge TL_CLK);               // At every positive edge of the TL clock
            end
        end
    endtask

    //TL related variables
    wire TL_CLK; reg TL_CLK_reg; assign TL_CLK = TL_CLK_reg;
    parameter TL_RATE = 1_000_000; 
    parameter TL_PERIOD = 1_000_000_000 / TL_RATE;
    initial TL_CLK_reg = 0; 
    always #(TL_PERIOD / 2) TL_CLK_reg = ~TL_CLK_reg;       // Generate the TL Clock
    wire TL_IN_VALID; assign TL_IN_VALID = 1'b1;            // For now, TSI data is always valid
    wire TL_IN_READY;
    wire TL_IN_BITS; 
    assign TL_IN_BITS = TSI_data_to_Adapter;                // Data into adapter is the TSI data bit

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
        // testchip to FPGA link - not testing in 
        // this module (see tl_transmitter_tb.v)
        .tl_out_valid(),
        .tl_out_ready(),
        .tl_out_bits(),
        // testchip to FPGA link
        .TL_IN_VALID(TL_IN_VALID),
        .TL_IN_READY(TL_IN_READY),
        .TL_IN_BITS(TL_IN_BITS)
    );
    
    initial begin
        // Hold reset
        rst = 1; repeat (10) @(posedge clk);
        // Delay for some time
        // No data should be transmitted
        rst = 0; repeat (10) @(posedge clk);

        while (!TL_IN_READY) BUTTONS_reg = BUTTONS_reg;     // Wait for adapter to be ready
        TSI_to_Adapter();                                   // Send TSI data
        UART_to_PC();                                       // Get data from the adapter
        
        rst = 0; repeat (10) @(posedge clk);                // Nothing should happen
        $finish();
    end

    initial begin
        cycle = 0;
        always @(posedge clk) begin
            cycle = cycle + 1;
        if (cycle > 10_000_000) $finish();                  // If too many cycles passed, terminate
                                                            // the program
        end
    end
endmodule