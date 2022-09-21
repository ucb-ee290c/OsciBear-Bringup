`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Daniel L.
// 
// Create Date: 09/17/2022 08:03:34 PM
// Design Name: 
// Module Name: adapter_controller 
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


module adapter_controller(
    input sysclk,
    input reset,
    // Controller Status
    output reg tx_controller_error,
    // TL Adapter Transmitter
    input tl_tx_busy,
    input tl_tx_done,
    input tl_tx_ready,
    output reg tl_tx_valid,
    output [7:0] tl_tx_data,
    output reg [7:0] tl_tx_length,
    // RX FIFO interface
    input [7:0] fifo_output,
    input fifo_empty,
    input fifo_full,
    output reg fifo_rd_en
);
localparam IDLE = 0, PARSING = 1, TRANSMITTING = 2;
localparam RECEIVING = 1;
reg fifo_empty_buff;
reg tx_state;
reg rx_state;

reg tx_fifo_rf;

assign tl_tx_data = fifo_output;

//////////////////////////
// Transmitter Controller
//////////////////////////
always @(posedge sysclk) begin
    if(reset) begin
        tl_tx_length <= 8'd0;
        fifo_rd_en  <= 0;
        tx_state <= IDLE;
        tx_fifo_rf = 0;
        tx_controller_error  <= 0;
    end
    case(tx_state)
        IDLE : begin
             if(fifo_empty_buff & ~fifo_empty) begin
                fifo_rd_en  <= 1; 
                tx_state <= PARSING;
             end
        end
        PARSING : begin 
            // FIFO output ready!
            tl_tx_length <= fifo_output;
            if(tl_tx_ready) begin
                // TL TX is ready
                tx_state <= TRANSMITTING;
                tl_tx_valid <= 1;
                fifo_rd_en <= 1;
            end
            else begin
                // TL TX is not ready
                tx_state <= PARSING;
                tl_tx_valid <= 0;
                fifo_rd_en <= 0;
            end
        end
        TRANSMITTING : begin
            if(tl_tx_done) begin
                tx_state <= IDLE;
                tl_tx_length <= 8'd0;    
            end
            else begin
                tl_tx_valid <= 0;
                // Ready for next byte 
                if(tl_tx_ready & ~tl_tx_valid) begin
                    fifo_rd_en  <= 1;
                    tl_tx_valid  <= 1;
                end
            end
        end
    endcase
end


endmodule
