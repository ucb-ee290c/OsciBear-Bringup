`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/26/2022 08:35:12 PM
// Design Name: 
// Module Name: chiptop
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


module chiptop(
    input CLK100MHZ,
    input uart_rx,
    output uart_tx
    );
    reg tx_reg;
    assign uart_tx = tx_reg;
    always @(posedge CLK100MHZ) begin
        tx_reg <= uart_rx;
    end
endmodule
