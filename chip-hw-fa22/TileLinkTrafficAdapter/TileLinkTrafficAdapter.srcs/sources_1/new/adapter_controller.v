`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/17/2022 08:03:34 PM
// Design Name: 
// Module Name: tl_controller
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


module adapter_controller(
    input sysclk,
    input reset,
    input tl_clk,
    input [7:0] fifo_output,
    input fifo_empty,
    input fifo_full,
    output fifo_rd_en
);
reg fifo_empty_buff;
always @(posedge sysclk) begin
    // FIFO no longer empty
    if(fifo_empty_buff & ~fifo_empty)
    
    end 
    
end


endmodule
