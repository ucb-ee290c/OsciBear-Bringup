`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: UC Berkeley
// Engineer: Daniel L. (referenced from Yufeng C.'s microcontroller approach)
// 
// Create Date: 09/17/2022 07:23:47 PM
// Design Name: 
// Module Name: tl_traffic_adapter
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


module tl_traffic_adapter#(
    parameter BAUD_RATE = 115_200
) (
    input sysclk,
    input reset,
    /// UART lines
    input uart_rx,
    output uart_tx,
    /// TileLink
    // Clock signal
    input tl_clk,
    // testchip to FPGA link
    input tl_out_valid,
    output tl_out_ready,
    input tl_out_bits,
    // FPGA to testchip link
    output tl_in_valid,
    input tl_in_ready,
    output tl_in_bits
);
    
    //////////////////////////////////////
    // On-chip UART
    //////////////////////////////////////
    //// UART Receiver
    wire [7:0] uart_rx_data_out;
    wire uart_rx_data_out_valid;
    wire uart_rx_data_out_ready;
    //// UART Transmitter
    reg [7:0] uart_tx_data_in;
    wire uart_tx_data_in_valid;
    wire uart_tx_data_in_ready;
    uart #(
        .CLOCK_FREQ(125_000_000),
        .BAUD_RATE(BAUD_RATE)
    ) on_chip_uart (
        .clk(clk),
        .reset(rst),

        .serial_in(serial_in),
        .data_out(uart_rx_data_out),
        .data_out_valid(uart_rx_data_out_valid),
        .data_out_ready(uart_rx_data_out_ready),

        .serial_out(serial_out),
        .data_in(uart_tx_data_in),
        .data_in_valid(uart_tx_data_in_valid),
        .data_in_ready(uart_tx_data_in_ready)
    );
    
    //////////////////////////////////
    // TileLink Adapter
    //////////////////////////////////
    // Controller
    tl_transmitter tl_transmitter;
    
    
    
    //////////////////////////////////
    // FIFO
    ////////////////////////////////// 
    wire fifo_wr_en;
    wire fifo_rd_en;
    wire fifo_empty;
    
    // Connect FIFO <-> UART Receiver
    assign fifo_wr_en = uart_rx_data_out_valid;
    assign uart_rx_data_out_ready = fifo_empty;
    
    fifo #(
        .WIDTH(8),
        .DEPTH(64)
    ) uart_fifo (
        .clk(sysclk),
        .rst(reset),
        .wr_en(fifo_wr_en),
        .din(uart_rx_data_out),
        .full(fifo_full),
        .rd_en(fifo_rd_en),
        .dout(fifo_dout),
        .empty(fifo_empty)
    );
    
    
endmodule
