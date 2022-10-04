
module tl_adapter(
  input clk,
  input rst,

  output [7:0]  tl_adapter_o_tx_data,
  output        tl_adapter_o_tx_valid,
  input         tl_adapter_i_tx_ready,
 
  input  [7:0]  tl_adapter_i_rx_data,
  input         tl_adapter_i_rx_valid,
  output        tl_adapter_o_rx_ready,

  output [2:0]  tl_chanid,
  output [2:0]  tl_a_opcode,
  output [2:0]  tl_a_param,
  output [3:0]  tl_a_size,
  output [3:0]  tl_a_source,
  output [31:0] tl_a_address,
  output [7:0]  tl_a_mask,
  output [63:0] tl_a_data,
  output        tl_a_corrupt,
  output        tl_a_valid,
  input         tl_a_ready

);


  reg [127:0] rx_fifo;
  reg [7:0]   rx_fifo_cnt;

  reg [127:0] tl_tx_fifo;




  assign tl_adapter_o_rx_ready = 'b1;
  assign tl_adapter_o_tx_data = 'b0;
  assign tl_adapter_o_tx_valid = 'b0;

  assign tl_a_valid = rx_fifo_cnt == 'd16;


  assign tl_chanid    = tl_tx_fifo[122:120];
  assign tl_a_opcode  = tl_tx_fifo[119:117];
  assign tl_a_param   = tl_tx_fifo[115:113];
  assign tl_a_size    = tl_tx_fifo[107:104];
  assign tl_a_source  = tl_tx_fifo[111:108];
  assign tl_a_address = tl_tx_fifo[95:64];
  assign tl_a_mask    = tl_tx_fifo[103:96];
  assign tl_a_data    = tl_tx_fifo[63:0];
  assign tl_a_corrupt = tl_tx_fifo[112];

  always @(posedge clk) begin
    if (!rst) begin
      rx_fifo <= 'h0;
      rx_fifo_cnt <= 'h0;
    end
    else begin
      if (tl_adapter_i_rx_valid && tl_adapter_o_rx_ready) begin
        rx_fifo <= {rx_fifo[127-8:0], tl_adapter_i_rx_data};
        rx_fifo_cnt <= rx_fifo_cnt + 'h1;
      end

      if (rx_fifo_cnt == 'd16) begin
        tl_tx_fifo <= rx_fifo;
        
      end
    end
  end


endmodule
