`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: UC Berkeley
// Engineer: Daniel Lovell
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


module z1top #(
    parameter BAUD_RATE = 115_200,
    /* verilator lint_off REALCVT */
    // Sample the button signal every 500us
    parameter integer B_SAMPLE_CNT_MAX = 0.0005 * CPU_CLOCK_FREQ,
    // The button is considered 'pressed' after 100ms of continuous pressing
    parameter integer B_PULSE_CNT_MAX = 0.100 / 0.0005
    /* lint_on */
) (
    input CLK_125MHZ_FPGA,
    input [3:0] BUTTONS,
    input [1:0] SWITCHES,
    output [5:0] LEDS,
    input  FPGA_SERIAL_RX,
    output FPGA_SERIAL_TX
);

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

endmodule
