`timescale 1ns / 1ns

module tb_basic();

  parameter CLOCK_FREQ = 100_000_000;
  parameter CLOCK_PERIOD = 1_000_000_000 / CLOCK_FREQ;

  reg clk, rst;
  initial clk = 'b0;
  always
    #(CLOCK_PERIOD/2) clk = ~clk;


  reg dut_uart_rx;
  wire dut_uart_tx;
  reg [7:0] dut_uart_tx_char;
  initial dut_uart_rx = 'b1; 
  
  reg [3:0]  tb_bram_mask;
  reg [9:0]  tb_bram_addr;
  reg [31:0] tb_bram_wdata;
  wire [31:0] tb_bram_rdata;
  
  task UART_Transmit;
    input [7:0] char;
    begin
      dut_uart_rx = 'b0;      // START
      #80;
      dut_uart_rx = char[0];  // bit 0
      #80;
      dut_uart_rx = char[1];  // bit 1
      #80;
      dut_uart_rx = char[2];  // bit 2
      #80;
      dut_uart_rx = char[3];  // bit 3
      #80;
      dut_uart_rx = char[4];  // bit 4
      #80;
      dut_uart_rx = char[5];  // bit 5
      #80;
      dut_uart_rx = char[6];  // bit 6
      #80;
      dut_uart_rx = char[7];  // bit 7
      #80;
      dut_uart_rx = 'b1;      // STOP
      #80;
    end
  endtask
  
  task UART_Receive;
    begin
      @(negedge dut_uart_tx);      // START
//      $display("%t [TEST]\tReceived DUT TX START.", $time);
      #40;
      #80;
      dut_uart_tx_char[0] = dut_uart_tx;  // bit 0
      #80;
      dut_uart_tx_char[1] = dut_uart_tx;  // bit 1
      #80;
      dut_uart_tx_char[2] = dut_uart_tx;  // bit 2
      #80;
      dut_uart_tx_char[3] = dut_uart_tx;  // bit 3
      #80;
      dut_uart_tx_char[4] = dut_uart_tx;  // bit 4
      #80;
      dut_uart_tx_char[5] = dut_uart_tx;  // bit 5
      #80;
      dut_uart_tx_char[6] = dut_uart_tx;  // bit 6
      #80;
      dut_uart_tx_char[7] = dut_uart_tx;  // bit 7
      #80;
      if (dut_uart_tx !== 'b1) begin
        $display("%t [TEST]\tDUT TX Frame ERROR.", $time);
      end
      ;      // STOP
      #40;
      $display("%t [TEST]\tReceived DUT TX %d / %h.", $time, dut_uart_tx_char, dut_uart_tx_char);
    end
  endtask



  top DUT (
    .CLK100MHZ(clk),
    .ck_rst(rst),
    .sw('b0),
    .led(),
    .btn('h0),
    .uart_rxd_out(dut_uart_tx),
    .uart_txd_in(dut_uart_rx)
  );
  
  reg [31:0] inst;

  initial begin
    #0;
    rst = 0;
    $display("[TEST]\tRESET pulled LOW.");
    repeat(2) @(posedge clk);
    @(negedge clk);
    rst = 1;
    $display("[TEST]\tRESET pulled HIGH.");
    
    repeat(8) @(posedge clk);
    
    #1;
    $display("%d [TEST]\t Issued PUTFULLDATA 0xdead_beef @ h0000_1000", $time);
    
    // fork
      begin
        UART_Transmit('b00001000);
        UART_Transmit('b00000000);
        UART_Transmit('b00000010);
        UART_Transmit('b00001111);

        UART_Transmit('h00);
        UART_Transmit('h00);
        UART_Transmit('h00);
        UART_Transmit('h01);

        UART_Transmit('hBA);
        UART_Transmit('hAD);
        UART_Transmit('hCA);
        UART_Transmit('hFE);
        
        UART_Transmit('hDE);
        UART_Transmit('hAD);
        UART_Transmit('hBE);
        UART_Transmit('hEF);
      end
      
    repeat(100) @(posedge clk);
    
    $finish();
      begin
        UART_Receive();
        UART_Receive();
        UART_Receive();
        UART_Receive();
        
//        UART_Receive();
//        UART_Receive();
//        UART_Receive();
//        UART_Receive();
      end
    // join
    
    repeat(8) @(posedge clk);
    
    #1;
//    $display("%d [TEST]\t Reading BRAM @ h0000_1000: %h", $time, DUT.bram_rdata);
  
    
    repeat(50) @(posedge clk);
    
    //if (DUT.u_bus_interface.u_tl_ram.u_ram.mem['h0000_1000-'h0000_1000] == 'hDEADBEEF) 
      //$display("[TEST]\tTest Passed.");
    //else 
      //$display("[TEST]\tTest Failed. Expected %d, actual %d", 'hDEADBEEF, DUT.u_bus_interface.u_tl_ram.u_ram.mem['h0000_1000-'h0000_1000]);
    
//    repeat(100) @(posedge clk);
    
    #1;
    inst = {'b0, 4'h2, 25'h0000_1000};
    $display("%d [TEST]\t Issued GET @ h0000_1000", $time);
//    inst = {`TL_A_OPCODE_GET, 4'h3, 25'h0000_000};
    fork
      begin
        UART_Transmit(inst[31:24]);
        UART_Transmit(inst[23:16]);
        UART_Transmit(inst[15:8]);
        UART_Transmit(inst[7:0]);
      end
      begin
        UART_Receive();
        UART_Receive();
        UART_Receive();
        UART_Receive();
        
        UART_Receive();
        UART_Receive();
        UART_Receive();
        UART_Receive();
      end
    join
    
    repeat(100) @(posedge clk);
    
    #1;
    inst = {'b0, 4'h2, 25'h0000_1008};
    $display("%d [TEST]\t Issued GET @ h0000_1008", $time);
////    inst = {`TL_A_OPCODE_GET, 4'h3, 25'h0000_000};
    fork
      begin
        UART_Transmit(inst[31:24]);
        UART_Transmit(inst[23:16]);
        UART_Transmit(inst[15:8]);
        UART_Transmit(inst[7:0]);
      end
      begin
        UART_Receive();
        UART_Receive();
        UART_Receive();
        UART_Receive();
        
        UART_Receive();
        UART_Receive();
        UART_Receive();
        UART_Receive();
      end
    join
    
    repeat(100) @(posedge clk);
    
    $finish();
  end
endmodule