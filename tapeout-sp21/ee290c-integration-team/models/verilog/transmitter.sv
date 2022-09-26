module transmitter(
	input clk,
        output Tx
);

  assign Tx = ~clk;
endmodule
