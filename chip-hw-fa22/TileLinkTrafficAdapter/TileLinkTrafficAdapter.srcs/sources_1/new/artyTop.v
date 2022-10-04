`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: UC Berkeley
// Engineer: Franklin H. Daniel L.
// 
// Create Date: 09/16/2022 06:16:51 PM
// Design Name: 
// Module Name: artyTop
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


module artyTop (
    input CLK100MHZ,
    input [3:0] btn,
    output [3:0] led,
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
    input TL_IN_BITS,
    // in case we need to test
    output CLK20MHZ
);
    clk_wiz_0(  .clk_in1(CLK100MHZ),
                .clk_out1(CLK20MHZ));
    ila_0(  .clk(CLK100MHZ),
            .probe0(uart_rx),
            .probe1(uart_tx),
            .probe2(TL_CLK),
            .probe3(TL_OUT_VALID),
            .probe4(TL_OUT_READY),
            .probe5(TL_OUT_BITS),
            .probe6(TL_IN_VALID),
            .probe7(TL_IN_READY),
            .probe8(TL_IN_BITS),
            .probe9(tl_tx_done));
    //// LED assignments
    // LED 4 is on when the bitstream is flashed
    wire tl_tx_busy, tl_tx_done, tl_rx_busy, tl_rx_done;
    assign led[0] = 1;
    assign led[1] = tl_tx_done;
    assign led[2] = tl_rx_busy;
    assign led[3] = tl_rx_done;

    parameter integer CLK_FREQ = 100_000_000;

    // Debounce resets
    // Sample period of the button
    parameter integer B_SAMPLE_CNT_MAX = 0.0005 * CLK_FREQ;
    // Button press interval to register edge
    parameter integer B_PULSE_CNT_MAX = 0.100 / 0.0005;
    
    // Buttons after the button_parser
    wire [3:0] buttons_pressed;
    button_parser #(
        .WIDTH(4),
        .SAMPLE_CNT_MAX(B_SAMPLE_CNT_MAX),
        .PULSE_CNT_MAX(B_PULSE_CNT_MAX)
    ) bp (
        .clk(CLK100MHZ),
        .in(btn),
        .out(buttons_pressed)
    );
    // Master reset signal
    assign reset = buttons_pressed[0];
    
    // Use IOBs to drive/sense the UART serial lines
    wire uart_tx, uart_rx;
    (* IOB = "true" *) reg fpga_serial_tx_iob;
    (* IOB = "true" *) reg fpga_serial_rx_iob;
    assign FPGA_SERIAL_TX = fpga_serial_tx_iob;
    assign uart_rx = fpga_serial_rx_iob;
    always @(posedge CLK100MHZ) begin
        fpga_serial_tx_iob <= uart_tx;
        fpga_serial_rx_iob <= FPGA_SERIAL_RX;
    end
    
    traffic_adapter #(
        .CLOCK_FREQ(CLK_FREQ),
        .BAUD_RATE(3_000_000)
    ) adapter (
        .sysclk(CLK100MHZ),
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
        .tl_in_bits(TL_IN_BITS),
         // Status bits
        //.tl_tx_busy(tl_tx_busy),
        .tl_tx_done(tl_tx_done),
        .tl_rx_busy(tl_rx_busy),
        .tl_rx_done(tl_rx_done)
    );

endmodule
