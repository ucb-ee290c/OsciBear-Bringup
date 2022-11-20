`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Franklin Huang
// 
// Create Date: 09/26/2022 08:35:12 PM
// Design Name: 
// Module Name: chiptop
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module chiptop (
    input CLK12MHZ,
    input rstBtn,
    output dutReset, // RESET is high when TSI/UART packets are all one
    output CLK20MHZ,
    output [0:0] led,
    // UART
    input uart_rx,
    output uart_tx,
    
    // TileLink Signals
    // Clock
    input tl_clk,
    // FPGA to testchip link
    output tl_out_valid,
    input tl_out_rd,
    output tl_out_data,
    // testchip to FPGA link
    input tl_in_valid,
    output tl_in_rd,
    input tl_in_data
);
    localparam 
        SYSCLK = 100_000_000,
        WIDTH = 123,
        BAUD = 2_000_000;

    wire tl_rising_clk, rst, triggerReset, CLK100MHZ;
    assign rst = rstBtn || dutReset;
    assign led[0] = rst;

    clk_wiz_0(.clk_in1(CLK12MHZ), 
              .clk_out1(CLK20MHZ), .clk_out2(CLK100MHZ));

    ila_0(.clk(CLK100MHZ), 
        .probe0(tl_rising_clk), .probe1(uart_rx),      .probe2(uart_tx), 
        .probe3(tl_clk),        .probe4(tl_out_valid), .probe5(tl_out_rd),
        .probe6(tl_out_data),   .probe7(tl_in_valid),  .probe8(tl_in_rd), 
        .probe9(tl_in_data));
            
    uartToTsi #(
        .SYSCLK(SYSCLK), 
        .WIDTH(WIDTH), 
        .BAUD(BAUD)) tx (
        .clk(CLK100MHZ),
        .rst(rst),
        .uart_rx(uart_rx),
        .tl_rising_clk(tl_rising_clk),
        .tl_out_rd(tl_out_rd),
        .tl_out_data(tl_out_data),
        .tl_out_valid(tl_out_valid),
        .RESET(triggerReset)
    );

    tsiToUart #(
        .SYSCLK(SYSCLK), 
        .WIDTH(WIDTH), 
        .BAUD(BAUD)) rx (
        .clk(CLK100MHZ),
        .rst(rst),
        .uart_tx(uart_tx),
        .tl_rising_clk(tl_rising_clk),
        .tl_in_rd(tl_in_rd),
        .tl_in_data(tl_in_data),
        .tl_in_valid(tl_in_valid)
    );

    EdgeDetector tlClkEdge (
        .clk(CLK100MHZ),
        .rst(rst),
        .in(tl_clk),
        .rising(tl_rising_clk)
        //.falling(),
    );
    
    // FSM that automatically specifies when the reset the DUT, unless triggered
    resetTop #(
        .TIME(100_000_000)) resetter (
        .clk(CLK100MHZ),
        .trigger(triggerReset),
        .dutReset(dutReset)
    );
    
endmodule
