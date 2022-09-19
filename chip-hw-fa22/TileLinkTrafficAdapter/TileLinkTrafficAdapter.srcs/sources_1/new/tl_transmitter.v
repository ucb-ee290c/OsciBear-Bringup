`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/17/2022 08:03:34 PM
// Design Name: 
// Module Name: tl_transmitter
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


module tl_transmitter(
    input sysclk,
    input reset,
    input tl_clk,
    input [7:0] fifo_output,
    output fifo_rd_en,
    input tl_tx_ready,
    output tl_tx_valid,
    output tx_done,
    output reg tl_out_bits
);

reg [7:0] tl_bit_ctr;
reg [3:0] fifo_byte_ctr;

wire posedge_tl_clk;
edge_detector #(
    .WIDTH(1)
) edge_detector (
    .clk(sysclk),
    .signal_in(tl_clk),
    .edge_detect_pulse(posedge_tl_clk)
);

always @ (posedge sysclk) begin
    if(fifo_rd_en) begin
        fifo_rd_en <= 0; // Enforce single-clock pulse
    end
    if(posedge_tl_clk) begin
        if(tl_tx_ready) begin
            tl_out_bits <= fifo_output[fifo_byte_ctr];
            tl_tx_valid <= 1;
            if(fifo_byte_ctr == 'd7) begin
                fifo_byte_ctr <= 'd0;
                fifo_rd_en <= 1;
            end
            else
                fifo_byte_ctr  <= fifo_byte_ctr + 1;
        end
        else begin
            tl_tx_valid <= 0;
        end            
    end
end

endmodule
