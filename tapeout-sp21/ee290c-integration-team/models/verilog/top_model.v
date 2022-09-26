// models DC offset and VGA atten feedback loops
// **note** adc changes on neg_edge, keep in mind for tb
`define SEEK_END 2

module top_model (
  // input real mixer_i,
  // input real mixer_q,
  input [5:0] current_dac_i_0,
  input [5:0] current_dac_i_1,
  input [5:0] current_dac_q_0,
  input [5:0] current_dac_q_1,
  input [9:0] vga_atten_i,
  input [9:0] vga_atten_q,

  output [7:0] adc_i,
  output [7:0] adc_q
);
// variable for testbench
integer fd;
integer i;
integer position;
integer size;
real mixer_i;
real mixer_q;
real fmixer_i[];
real fmixer_q[];
reg clock = 1'b0;

chain_model i_chain (
  .mixer_input(mixer_i),
  .current_dac_0(current_dac_i_0),
  .current_dac_1(current_dac_i_1),
  .vga_atten(vga_atten_i),
  .adc_out(adc_i)
);

chain_model q_chain (
  .mixer_input(mixer_q),
  .current_dac_0(current_dac_q_0),
  .current_dac_1(current_dac_q_1),
  .vga_atten(vga_atten_q),
  .adc_out(adc_q)
);

always #(`CLOCK_PERIOD/2.0) clock = ~clock;

initial begin
    
    // get line size
    fd = $fopen("./mixer_input.txt", "r");
    i = $fseek(fd, 0, `SEEK_END); /* End of file */
    position = $ftell(fd);
    size = (position + 1) / 30;
    $fclose(fd);

    // init dynamic array
    fmixer_i = new[size];
    fmixer_q = new[size];

    // reading I/Q from file
    fd = $fopen("./mixer_input.txt", "r");
    i = 0;
    while ($fscanf(fd, "%f %f", fmixer_i[i], fmixer_q[i]) ==2) begin
      // $display("i: %f, q: %f", mixer_i, mixer_q);
      i = i + 1;
    end
    
    // sending attenna data
    while(1) begin
      for(i = 0; i < size; i = i + 1) begin
        @(negedge clock);
        mixer_i = fmixer_i[i];
        mixer_q = fmixer_q[i];
        $display("i: %f, q: %f", mixer_i, mixer_q);
      end
    end
    $fclose(fd);
    $finish;
end

endmodule 
