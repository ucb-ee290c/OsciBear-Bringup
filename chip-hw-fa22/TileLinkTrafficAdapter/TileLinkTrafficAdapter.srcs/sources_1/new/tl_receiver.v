`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Daniel L.
// 
// Create Date: 09/17/2022 08:03:34 PM
// Design Name: 
// Module Name: tl_receiver
// Project Name: TL Adapter
// Target Devices: Pynq Z1, Arty, CMOD A7
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


module tl_receiver(
    input sysclk,
    input reset,
    // TileLink Bus
    input tl_clk,
    input tl_in_valid,
    output reg tl_out_ready,
    input tl_in_bits,
    // Interface with the Adapter Controller
    output [7:0] tl_rx_data,
    output reg tl_rx_valid,
    input tl_rx_ready,
    // Recceiver state
    output reg tx_busy,
    output reg tx_done
);


wire posedge_tl_clk;
edge_detector #(
    .WIDTH(1)
) edge_detector (
    .clk(sysclk),
    .signal_in(tl_clk),
    .edge_detect_pulse(posedge_tl_clk)
);

localparam IDLE = 0, RECEIVING = 1;

reg cur_state;
reg nxt_state;
reg [3:0] fifo_byte_ctr;

always @(posedge sysclk) begin
    if(reset) begin
        state <= IDLE;
        fifo_byte_ctr  <= 'd0;
    end
    else begin
        case  
    end
end


endmodule
