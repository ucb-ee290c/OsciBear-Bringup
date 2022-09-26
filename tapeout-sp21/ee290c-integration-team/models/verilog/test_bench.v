//`timescale 1ns/10ps
module test_bench();

  //outputs
  wire Tx, JTAG_TDI, SPI_DQ_0, SPI_DQ_1;

  //inputs/inout
  reg Rx, clk_A, clk_D, Analog_Test_1, Analog_Test_2, Analog_Test_3, Analog_Test_4, MSEL, JTAG_TCK, JTAG_TDO, JTAG_TMS, SPI_CS, SPI_SCK, SPI_DQ_2, SPI_DQ_3, GPIO_1, GPIO_2, GPIO_3, GPIO_4, GPIO_5, GPIO_6, dbb_off_chip_mode, RESET, CLK_OVER, V_BAT, V_REF, I_REF, GND;
 
  initial clk_A = 0;
  initial clk_D = 0;
  always #(10) clk_A <= ~clk_A;
  always #(10) clk_D <= ~clk_D;

  chip_top SoC(
   	.Tx(Tx),
	.Rx(Rx),
    	.Analog_Test_1(Analog_Test_1),
    	.Analog_Test_2(Analog_Test_2),
    	.Analog_Test_3(Analog_Test_3),
    	.Analog_Test_4(Analog_test_4),
    	.MSEL(MSEL),
        .JTAG_TCK(JTAG_TCK),
   	.JTAG_TDO(JTAG_TDO),
    	.JTAG_TMS(JTAG_TMS),
    	.JTAG_TDI(JTAG_TDI),
    	.SPI_CS(SPI_CS),
    	.SPI_SCK(SPI_SCK),
    	.SPI_DQ_0(SPI_DQ_0),
    	.SPI_DQ_1(SPI_DQ_1),
    	.SPI_DQ_2(SPI_DQ-2),
    	.SPI_DQ_3(SPI_DQ_3),
    	.GPIO_1(GPIO_1),
    	.GPIO_2(GPIO_2),
    	.GPIO_3(GPIO_3),
    	.GPIO_4(GPIO_4),
    	.GPIO_5(GPIO_5),
   	.GPIO_6(GPIO_6),
	.dbb_off_chip_mode(dbb_off_chip_mode),
    	.RESET(RESET),
    	.CPU_REF_CLK(clk_D),
    	.RF_REF_CLK(clk_A),
    	.CLK_OVER(CLK_OVER),
    	.V_BAT(V_BAT),
    	.V_REF(V_REF),
    	.I_REF(I_REF),
    	.GND(GND)
  );

  transmitter TX(.clk(clk_A), .Tx(Rx));

  initial begin

    RESET = 1;
    #10;
    $display("%d", SPI_DQ_0);
  end
  
endmodule
