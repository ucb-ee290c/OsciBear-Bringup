module dc_model (
    output real voltage_data_out,
    input real voltage_data_in,
    input [5:0] current_dac
);

    real offset;
    assign offset = (current_dac[5] == 1'b0) ? current_dac[4:0] * 0.195 : current_dac[4:0]  * -0.195;
    assign voltage_data_out = voltage_data_in + offset;

endmodule


// module tb();
    

//     real out;
//     real in;
//     reg [5:0] x;

//     dc_model model (
//         .voltage_data_out(out),
//         .voltage_data_in(in),
//         .current_dac(x)
//     );

//     integer i;
//     assign in = 0;
//     initial begin
//         for (i = 0; i < 64; i = i + 1) begin
//             x = i;
//             #(1);
//             $display("%b %f\n", x, out);
//         end
//     end
// endmodule
