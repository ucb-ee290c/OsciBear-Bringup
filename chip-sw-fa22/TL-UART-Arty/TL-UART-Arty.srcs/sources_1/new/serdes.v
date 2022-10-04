`timescale 1ns / 1ps


module serdes(
  input clk,
  input rst,

  input  [2:0]  tl_a_opcode,
  input  [2:0]  tl_a_param,
  input  [3:0]  tl_a_size,
  input  [3:0]  tl_a_source,
  input  [31:0] tl_a_address,
  input  [7:0]  tl_a_mask,
  input  [63:0] tl_a_data,
  input         tl_a_corrupt,
  input         tl_a_valid,
  output        tl_a_ready,

  output        tsi_mosi_data,
  output        tsi_mosi_valid,
  intput        tsi_mosi_ready
);

  always @(posedge clk) begin
    if (!rst) begin

    end
    else begin

    end
  end



endmodule
