`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: UC Berkeley
// Engineer: Daniel L.
// 
// Create Date: 09/16/2022 06:16:51 PM
// Design Name: 
// Module Name: z1top
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


module top #(
    /* verilator lint_off REALCVT */
    parameter integer CLK_FREQ = 125_000_000,
    // Sample period of the button
    parameter integer B_SAMPLE_CNT_MAX = 0.0005 * CLK_FREQ,
    // Button press interval to register edge
    parameter integer B_PULSE_CNT_MAX = 0.100 / 0.0005
    /* lint_on */
) (
    input CLK_125MHZ_FPGA,
    input [3:0] BUTTONS,
    input [1:0] SWITCHES,
    output [5:0] LEDS,
    input  FPGA_SERIAL_RX,
    output FPGA_SERIAL_TX,
    // TileLink
    // Clock signal
    input TL_CLK,
    // FPGA to testchip link
    output TL_OUT_VALID,
    input TL_OUT_READY,
    output TL_OUT_BITS,
    // testchip to FPGA link
    input TL_IN_VALID,
    output TL_IN_READY,
    input TL_IN_BITS
);

    // Debounce resets

    // Use IOBs to drive/sense the UART serial lines
    wire uart_tx, uart_rx;
    (* IOB = "true" *) reg fpga_serial_tx_iob;
    (* IOB = "true" *) reg fpga_serial_rx_iob;
    assign FPGA_SERIAL_TX = fpga_serial_tx_iob;
    assign uart_rx = fpga_serial_rx_iob;
    always @(posedge CLK_125MHZ_FPGA) begin
        fpga_serial_tx_iob <= uart_tx;
        fpga_serial_rx_iob <= FPGA_SERIAL_RX;
    end
    
    traffic_adapter #(
        .BAUD_RATE(115_200)
    ) adapter (
        .sysclk(CLK_125MHZ_FPGA),
        .reset(reset),
        /// UART lines
        .uart_rx(uart_rx),
        .uart_tx(uart_tx),
        /// TileLink
        // Clock signal
        .tl_clk(TL_CLK),
        // testchip to FPGA link
        .tl_out_valid(TL_OUT_VALID),
        .tl_out_ready(TL_OUT_READY),
        .tl_out_bits(TL_OUT_BITS),
        // FPGA to testchip link
        .tl_in_valid(TL_IN_VALID),
        .tl_in_ready(TL_IN_READY),
        .tl_in_bits(TL_IN_BITS)
    );

endmodule
