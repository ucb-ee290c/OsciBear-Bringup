`define GAIN1(x) \
   (x == 1'd0) ? 1.296537253799 : \
                 23.131297805639

module vga_model1 (
  input  real in, // "sampled" differential voltage, clipped between [0, 0.9]
  input       code3, // 0 = 2.2557 dB, 1 = 27.284 dB
  output real out
);
  
  // map code to gain
  real gain;
  assign gain = `GAIN1(code3); 

  real prod;
  assign prod = in * gain;

  assign out = prod < 0.9 ? 0.9 : prod;

endmodule
