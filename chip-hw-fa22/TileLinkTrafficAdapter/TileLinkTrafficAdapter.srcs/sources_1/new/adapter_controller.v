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
    // TL Adapter Receiver
    input tl_rx_busy,
    input tl_rx_done,
    output reg tl_rx_ready,
    input tl_rx_valid,
    input [7:0] tl_rx_data,
    // TX FIFO interface
    output reg [7:0] tx_fifo_input,
    input tx_fifo_empty,
    input tx_fifo_full,
    output reg tx_fifo_wr_en,
    // RX FIFO interface
    input [7:0] rx_fifo_output,
    input rx_fifo_empty,
    input rx_fifo_full,
    output reg rx_fifo_rd_en
);
localparam IDLE = 0, PARSING = 1, TRANSMITTING = 2;
localparam RECEIVING = 1;
reg rx_fifo_empty_buff;
reg [1:0] tx_state;

reg tx_fifo_rf;

assign tl_tx_data = rx_fifo_output;

// Buffered FIFO Empty signal 
always @(posedge sysclk) begin
    rx_fifo_empty_buff <= rx_fifo_empty;
end

//////////////////////////
// Transmitter Controller
//////////////////////////
always @(posedge sysclk) begin
    if(reset) begin
        tl_tx_length <= 8'd0;
        rx_fifo_rd_en  <= 0;
        tx_state <= IDLE;
        tx_fifo_rf = 0;
        tx_controller_error  <= 0;
        tl_tx_valid <= 0;
    end
    case(tx_state)
        IDLE : begin
             if(rx_fifo_empty_buff & ~rx_fifo_empty) begin
                rx_fifo_rd_en  <= 1; 
                tx_state <= PARSING;
             end
        end
        PARSING : begin 
            // FIFO output ready!
            tl_tx_length <= rx_fifo_output;
            if(tl_tx_ready) begin
                // TL TX is ready
                tx_state <= TRANSMITTING;
                tl_tx_valid <= 1;
                rx_fifo_rd_en <= 1;
            end
            else begin
                // TL TX is not ready
                tx_state <= PARSING;
                tl_tx_valid <= 0;
                rx_fifo_rd_en <= 0;
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
                    rx_fifo_rd_en  <= 1;
                    tl_tx_valid  <= 1;
                end
            end
        end
    endcase
end

//////////////////////////
// Receiver Controller
//////////////////////////
always @(posedge sysclk) begin
    if(reset) begin 
        tl_rx_ready <= 0;
        tx_fifo_wr_en <= 0;
        tx_fifo_input <= 'd0;
    end
    else begin 
        if(~tx_fifo_full) begin
            tl_rx_ready <= 1;
        end
        else begin 
            tl_rx_ready <= 0;
        end
        if(tl_rx_valid && tl_rx_ready) begin
            tx_fifo_input <= tl_rx_data;
            tx_fifo_wr_en <= 1;
        end
        else begin
            tx_fifo_wr_en <= 0;
        end
    end
end


endmodule
