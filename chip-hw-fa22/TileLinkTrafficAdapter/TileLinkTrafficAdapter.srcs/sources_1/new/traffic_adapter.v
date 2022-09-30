`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: UC Berkeley
// Engineer: Daniel L. (referenced from Yufeng C.'s microcontroller approach)
// 
// Create Date: 09/17/2022 07:23:47 PM
// Design Name: Traffic Adapter
// Module Name: traffic_adapter
// Project Name: 
// Target Devices: PYNQ Z1, Arty, CMOD A7
// Tool Versions: Vivado 2022.1
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module traffic_adapter#(
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
    // FPGA to testchip link
    output tl_out_valid,
    input tl_out_ready,
    output tl_out_bits,
    // testchip to FPGA link
    input tl_in_valid,
    output tl_in_ready,
    input tl_in_bits,
    // Status bits
    output tl_tx_done,
    output tl_rx_busy,
    output tl_rx_done
);
    
    //////////////////////////////////////
    // On-chip UART
    //////////////////////////////////////
    //// UART Receiver
    wire [7:0] uart_rx_data_out;
    wire uart_rx_data_out_valid;
    wire uart_rx_data_out_ready;
    //// UART Transmitter
    wire [7:0] uart_tx_data_in;
    reg uart_tx_data_in_valid;
    wire uart_tx_data_in_ready;
    uart #(
        .CLOCK_FREQ(125_000_000),
        .BAUD_RATE(BAUD_RATE)
    ) on_chip_uart (
        .clk(sysclk),
        .reset(reset),

        .serial_in(uart_rx),
        .data_out(uart_rx_data_out),
        .data_out_valid(uart_rx_data_out_valid),
        .data_out_ready(uart_rx_data_out_ready),

        .serial_out(uart_tx),
        .data_in(uart_tx_data_in),
        .data_in_valid(uart_tx_data_in_valid),
        .data_in_ready(uart_tx_data_in_ready)
    );
    
    //////////////////////////////////
    // FIFO
    //////////////////////////////////
    
    // RX FIFO 
    wire rx_fifo_wr_en;
    wire rx_fifo_rd_en;
    wire rx_fifo_empty;
    wire rx_fifo_full;
    wire [7:0] rx_fifo_dout;
    
    // Connect FIFO <-> UART Receiver
    assign rx_fifo_wr_en = uart_rx_data_out_valid;
    assign uart_rx_data_out_ready = ~rx_fifo_full;
    
    fifo #(
        .WIDTH(8),
        .DEPTH(64)
    ) rx_fifo (
        .clk(sysclk),
        .rst(reset),
        .wr_en(rx_fifo_wr_en),
        .din(uart_rx_data_out),
        .full(rx_fifo_full),
        .rd_en(rx_fifo_rd_en),
        .dout(rx_fifo_dout),    // UART RX output
        .empty(rx_fifo_empty)
    );
    
    // TX FIFO 
    wire tx_fifo_wr_en;
    reg tx_fifo_rd_en;
    wire tx_fifo_empty;
    wire tx_fifo_full;
    wire [7:0] tx_fifo_din;
    
    // Connect FIFO <-> UART Transmitter
    always @(*) begin
        if(~tx_fifo_empty && uart_tx_data_in_ready) begin
            tx_fifo_rd_en = 1;
        end
        else begin 
            // UART should make a copy of din for himself
            tx_fifo_rd_en = 0;
        end
    end
    always @(posedge sysclk) begin
        if(~tx_fifo_empty && uart_tx_data_in_ready) begin
            uart_tx_data_in_valid <= 1; // This might cause trouble.. Need to wait?
        end
        else begin
            uart_tx_data_in_valid <= 0;
        end
    end
    
    fifo #(
        .WIDTH(8),
        .DEPTH(64)
    ) tx_fifo (
        .clk(sysclk),
        .rst(reset),
        .wr_en(tx_fifo_wr_en),
        .din(tx_fifo_din),
        .full(tx_fifo_full),
        .rd_en(tx_fifo_rd_en),
        .dout(uart_tx_data_in), // UART TX input
        .empty(tx_fifo_empty)
    );  
    
    
     //////////////////////////////////
    // TileLink Adapter
    //////////////////////////////////
    // TL Adapter Transmitter nets
    wire tl_tx_ready, tl_tx_valid;
    wire [7:0] tl_tx_data;
    // TL Adapter Receiver nets
    wire tl_rx_ready, tl_rx_valid;
    wire [7:0] tl_rx_data;
    // Controller nets
    wire tx_controller_error;
    wire [7:0] tl_tx_length;
    
    // Controller
    adapter_controller adapter_controller (
        .sysclk(sysclk),
        .reset(reset),
        // Controller Status
        .tx_controller_error(tx_controller_error),
        // TL Adapter Transmitter
        .tl_tx_done(tl_tx_done),
        .tl_tx_ready(tl_tx_ready),
        .tl_tx_valid(tl_tx_valid),
        .tl_tx_data(tl_tx_data),
        .tl_tx_length(tl_tx_length),
        // TL Adapter Receiver
        .tl_rx_busy(tl_rx_busy),
        .tl_rx_done(tl_rx_done),
        .tl_rx_ready(tl_rx_ready),
        .tl_rx_valid(tl_rx_valid),
        .tl_rx_data(tl_rx_data),
        // TX FIFO interface
        .tx_fifo_input(tx_fifo_din),
        .tx_fifo_empty(tx_fifo_empty),
        .tx_fifo_full(tx_fifo_full),
        .tx_fifo_wr_en(tx_fifo_wr_en),
        // RX FIFO interface
        .rx_fifo_output(rx_fifo_dout),
        .rx_fifo_empty(rx_fifo_empty),
        .rx_fifo_full(rx_fifo_full),
        .rx_fifo_rd_en(rx_fifo_rd_en)
    );
    
    // Transmitter
    tl_transmitter tl_transmitter(
        .sysclk(sysclk),
        .reset(reset),
        // TileLink Bus
        .tl_clk(tl_clk),
        .tl_out_ready(tl_out_ready),
        .tl_out_valid(tl_out_valid),
        .tl_out_bits(tl_out_bits),
        // Interface with the Adapter Controller
        .tl_tx_length(tl_tx_length),
        .tl_tx_data(tl_tx_data),
        .tl_tx_valid(tl_tx_valid),
        .tl_tx_ready(tl_tx_ready),
        // Transmitter state
        .tx_done(tl_tx_done)
    );
    // Receiver
    tl_receiver tl_receiver(
        .sysclk(sysclk),
        .reset(reset),
        // TileLink Bus
        .tl_clk(tl_clk),
        .tl_in_valid(tl_in_valid),
        .tl_in_ready(tl_in_ready),
        .tl_in_bits(tl_in_bits),
        // Interface with the Adapter Controller
        .tl_rx_data(tl_rx_data),
        .tl_rx_valid(tl_rx_valid),
        .tl_rx_ready(tl_rx_ready),
        // Receiver state
        .rx_busy(tl_rx_busy),
        .rx_done(tl_rx_done)
    );

    
endmodule
