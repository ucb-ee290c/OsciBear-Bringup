module chain_model (
  input real mixer_input, // sampled differential voltage

  input [5:0] current_dac_0,
  input [5:0] current_dac_1,
  input [9:0] vga_atten,

  output [7:0] adc_out
);

wire [3:0] code;
// todo: map vga_atten to code
// assign code = f(vga_atten); 10 bit to 4 bit map?

real dc_model_1_out;
real vga_model_1_out;
real dc_model_2_out;
real vga_model_2_out;

dc_model dc_model_1 (
  .current_dac(current_dac_0),
  .voltage_data_in(mixer_input),
  .voltage_data_out(dc_model_1_out)
);

vga_model1 vga_model_1 (
  .in(dc_model_1_out),
  .code3(code[3]),
  .out(vga_model_1_out)
);

// ignore bp filter for now

dc_model dc_model_2 (
  .current_dac(current_dac_1),
  .voltage_data_in(vga_model_1_out),
  .voltage_data_out(dc_model_2_out)
);

vga_model2 vga_model_2 (
  .in(dc_model_2_out),
  .code(code),
  .out(vga_model_2_out)
);

// discretize output of chain
assign adc_out = (vga_model_2_out / 0.9) * 256; 

endmodule
