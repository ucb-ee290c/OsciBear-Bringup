`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Daniel L.
// 
// Create Date: 09/17/2022 08:03:34 PM
// Design Name: 
// Module Name: tl_transmitter
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


module tl_transmitter(
    input sysclk,
    input reset,
    // TileLink Bus
    input tl_clk,
    input tl_out_ready,
    output reg tl_out_valid,
    output reg tl_out_bits,
    // Interface with the Adapter Controller
    input [7:0] tl_tx_length,
    input [7:0] tl_tx_data,
    input tl_tx_valid,
    output reg tl_tx_ready,
    // Transmitter state
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

localparam IDLE = 0, TRANSMITTING = 1, EXITING = 2;
reg [1:0] state;
reg tx_buffered_length;

reg [7:0] bit_ctr;
reg [3:0] fifo_byte_ctr;

always @ (posedge sysclk) begin
    if(reset) begin
        state <= IDLE;
        tx_buffered_length <= 8'd0;
        bit_ctr  <= 'd0;
        fifo_byte_ctr <= 'd0;
    end 
    else begin 
    case (state) 
        IDLE : begin
            if(tl_tx_valid) begin
                state <= TRANSMITTING;
                tx_buffered_length  <= tl_tx_length ;             
            end
        
        end
        TRANSMITTING  : begin
            if(posedge_tl_clk && (tl_out_ready)) begin
                tl_out_bits <= tl_tx_data[fifo_byte_ctr];
                tl_out_valid  <= 1;
                bit_ctr <= bit_ctr + 1;     
                if(bit_ctr >= (tx_buffered_length - 1)) begin
                    state <= EXITING;
                    tx_buffered_length <= 8'd0;
                    bit_ctr  <= 'd0;
                    fifo_byte_ctr <= 'd0;
                    tl_tx_ready <= 0;
                end
                else begin
                    fifo_byte_ctr <= fifo_byte_ctr  + 1;
                    if(fifo_byte_ctr == 'd7) begin
                        fifo_byte_ctr <= 'd0;
                        tl_tx_ready <= 1;
                    end
                end
            end
        end
        EXITING : begin
            // Need a single TL clk cycle to ensure that the value is stable
            if(posedge_tl_clk) begin
                tl_out_valid <= 0;
                state <= IDLE;
                tl_tx_ready <= 0;
            end
        end
    endcase
    end
end

endmodule
