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
    input [7:0] fifo_output,
    output fifo_rd_en,
    input tl_tx_ready,
    output tl_tx_valid,
    output tx_done,
    output tl_out_bits,
);

localparam IDLE = 0 
    FRAME_LAST = 1, 
    FRAME_MASK = 2, 
    FRAME_CORRUPT = 3, 
    FRAME_DATA = 4,
    FRAME_ADDRESS = 5, 
    FRAME_SOURCE = 6, 
    FRAME_SIZE = 7, 
    FRAME_PARAM = 8, 
    FRAME_OPCODE = 9,
    FRAME_CHANID = 10;

localparam FRAME_LAST_SIZE = 1, 
    FRAME_MASK_SIZE = 8, 
    FRAME_CORRUPT_SIZE = 1, 
    FRAME_DATA_SIZE = 64,
    FRAME_ADDRESS_SIZE = 32, 
    FRAME_SOURCE_SIZE = 4,
    FRAME_SIZE_SIZE = 4, 
    FRAME_PARAM_SIZE = 3, 
    FRAME_OPCODE_SIZE = 3, 
    FRAME_CHANID_SIZE = 3;   
    
reg [7:0] tl_bit_ctr;
reg [3:0] fifo_byte_ctr;
reg [3:0] cur_state;
reg {3:0] next_state;


always @ posedge(sysclk) begin
    if (reset) 
        cur_state <= IDLE;
    else
        cur_state <= next_state;
end

always @(*) begin
    if(tl_tx_ready) begin
        tl_out_bits = fifo_output[fifo_byte_ctr];
        case(cur_state)
            IDLE : begin 
                next_state = FRAME_LAST;
            end
            FRAME_LAST : begin 
                next_state = FRAME_MASK;
            end
            FRAME_MASK : begin
                next_state = FRAME_CORRUPT;
            end
            FRAME_CORRUPT : begin
                next_state = FRAME_DATA;
            end
            FRAME_DATA : begin
                next_state = FRAME_ADDRESS;
            end
            FRAME_ADDRESS : begin 
                next_state = FRAME_SOURCE;
            end
            FRAME_SOURCE : begin
                next_state = FRAME_SIZE;
            end
            FRAME_SIZE : begin 
                next_state = FRAME_PARAM;
            end
            FRAME_PARAM : begin 
                next_state = FRAME_OPCODE;
            end
            FRAME_OPCODE : begin 
                next_state = FRAME_CHANID;
            end
            FRAME_CHANID : begin 
                next_state = IDLE;
            end
        endcase 
    end

end


endmodule
