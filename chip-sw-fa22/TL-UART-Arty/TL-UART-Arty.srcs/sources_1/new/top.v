`timescale 1ns / 1ns


module top(
  input         CLK100MHZ,
  input         ck_rst,
  input   [3:0] sw,
  output  [3:0] led,
  input   [3:0] btn,
  
  output        uart_rxd_out,
  input         uart_txd_in
  
);

  wire [7:0]  uart2tl_rx_data;
  wire        uart2tl_rx_valid;
  wire        tl2uart_rx_ready;
  wire [7:0]  tl2uart_tx_data;
  wire        tl2uart_tx_valid;
  wire        uart2tl_tx_ready;

  wire [2:0]  tl2serdes_chanid;
  wire [2:0]  tl2serdes_a_opcode;
  wire [2:0]  tl2serdes_a_param;
  wire [3:0]  tl2serdes_a_size;
  wire [3:0]  tl2serdes_a_source;
  wire [31:0] tl2serdes_a_address;
  wire [7:0]  tl2serdes_a_mask;
  wire [63:0] tl2serdes_a_data;
  wire        tl2serdes_a_corrupt;
  wire        tl2serdes_a_valid;
  wire        serdes2tl_a_ready;
    
  uart u_uart (
    .clk(CLK100MHZ),
    .rst(ck_rst),

    .uart_i_tx_data(tl2uart_tx_data),
    .uart_i_tx_valid(tl2uart_tx_valid),
    .uart_o_tx_ready(uart2tl_tx_ready),

    .uart_o_rx_data(uart2tl_rx_data),
    .uart_o_rx_valid(uart2tl_rx_valid),
    .uart_i_rx_ready(tl2uart_rx_ready),

    .rxd(uart_txd_in),
    .txd(uart_rxd_out),

    .tx_busy(),
    .rx_busy(),
    .rx_overrun_error(),
    .rx_frame_error(),

    .prescale('d1)
  );

  tl_adapter u_tl_adapter (
    .clk(CLK100MHZ),
    .rst(ck_rst),

    .tl_adapter_o_tx_data(tl2uart_tx_data),
    .tl_adapter_o_tx_valid(tl2uart_tx_valid),
    .tl_adapter_i_tx_ready(uart2tl_tx_ready),

    .tl_adapter_i_rx_data(uart2tl_rx_data),
    .tl_adapter_i_rx_valid(uart2tl_rx_valid),
    .tl_adapter_o_rx_ready(tl2uart_rx_ready),

    
    .tl_chanid(),
    .tl_a_opcode(tl2serdes_a_opcode),
    .tl_a_param(tl2serdes_a_param),
    .tl_a_size(tl2serdes_a_size),
    .tl_a_source(tl2serdes_a_source),
    .tl_a_address(tl2serdes_a_address),
    .tl_a_mask(tl2serdes_a_mask),
    .tl_a_data(tl2serdes_a_data),
    .tl_a_corrupt(tl2serdes_a_corrupt),
    .tl_a_valid(tl2serdes_a_valid),
    .tl_a_ready(serdes2tl_a_ready)
  );

  serdes u_serdes (
    .clk(CLK100MHZ),
    .rst(ck_rst),

    .tl_a_opcode(tl2serdes_a_opcode),
    .tl_a_param(tl2serdes_a_param),
    .tl_a_size(tl2serdes_a_size),
    .tl_a_source(tl2serdes_a_source),
    .tl_a_address(tl2serdes_a_address),
    .tl_a_mask(tl2serdes_a_mask),
    .tl_a_data(tl2serdes_a_data),
    .tl_a_corrupt(tl2serdes_a_corrupt),
    .tl_a_valid(tl2serdes_a_valid),
    .tl_a_ready(serdes2tl_a_ready),

    .tsi_mosi_data(),
    .tsi_mosi_valid(),
    .tsi_mosi_ready('b1)
  )

endmodule
