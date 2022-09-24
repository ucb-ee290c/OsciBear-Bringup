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
    output reg tl_in_ready,
    input tl_in_bits,
    // Interface with the Adapter Controller
    output [7:0] tl_rx_data,
    output reg tl_rx_valid,
    input tl_rx_ready,
    // Receiver state
    output reg rx_busy,
    output reg rx_done
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

reg state;
reg [3:0] tl_byte_ctr;
reg [7:0] tl_rx_byte;

assign tl_rx_data = tl_rx_byte;

always @(posedge sysclk) begin
    if(reset) begin
        rx_busy <= 0;
        rx_done <= 1;
        state <= IDLE;
        tl_byte_ctr  <= 'd0;
        tl_rx_byte <= 'd0;
    end
    else begin
        case(state) 
        IDLE : begin
            tl_in_ready <= 1;
            tl_rx_valid <= 0;
            if(posedge_tl_clk && tl_in_valid) begin
                rx_busy <= 1;
                state <= RECEIVING;
                tl_byte_ctr <= 'd1;
                tl_rx_byte <= {7'b0, tl_in_bits};
            end
            else begin 
                state <= IDLE;
                tl_byte_ctr <= 'd0;
                tl_rx_byte <= 8'b0;
            end
        end
        RECEIVING : begin
            
            tl_in_ready <= 1;
            tl_rx_valid <= 0;
            if(posedge_tl_clk) begin
                if(tl_in_valid) begin
                    tl_rx_byte <= tl_rx_byte  | (tl_in_bits  << tl_byte_ctr);
                    tl_byte_ctr <= tl_byte_ctr + 1; 
                    if(tl_byte_ctr == 'd7) begin
                        // Need to wait a cycle to propagate the last bit
                        // to output of tl_rx_byte
                        tl_rx_valid <= 1;
                        tl_byte_ctr <= 8'b0;
                    end
                end
            end
        end
        endcase
    end
end

endmodule
