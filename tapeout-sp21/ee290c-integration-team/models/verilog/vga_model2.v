`define GAIN1(x) \
   (x == 1'd0) ? 1.296537253799 : \
                 23.131297805639

`define GAIN(x) \
   (x == 4'd0) ? 1.296537253799 : \
   (x == 4'd1) ? 2.0753435518561 : \
   (x == 4'd2) ? 3.1134005657774 : \
   (x == 4'd3) ? 4.6163635154047 : \
   (x == 4'd4) ? 7.3815913723952 : \
   (x == 4'd5) ? 10.036909309201 : \
   (x == 4'd6) ? 12.765857727198 : \
   (x == 4'd7) ? 20.206914319923 : \
   (x == 4'd8) ? 23.131297805639 : \
   (x == 4'd9) ? 37.025420540014 : \
   (x == 4'd10) ? 55.545643184739 : \
   (x == 4'd11) ? 82.356901715361 : \
   (x == 4'd12) ? 131.68915167332 : \
   (x == 4'd13) ? 179.08120170201 : \
   (x == 4'd14) ? 227.79804889567 : \
                  360.57864302164

module vga_model2 (
  input  real       in, // "sampled" differential voltage, clipped between [0, 0.9]
  input       [3:0] code,
  output real       out
);

  // map code to gain
  real gain1;
  assign gain1 = `GAIN1(code[3]);
  real gain;
  assign gain = `GAIN(code) / gain1; 

  real prod;
  assign prod = in * gain;

  assign out = prod < 0.9 ? 0.9 : prod;

endmodule
