module edge_detector #(
    parameter WIDTH = 1
)(
    input clk,
    input [WIDTH-1:0] signal_in,
    output [WIDTH-1:0] edge_detect_pulse
);
    reg [WIDTH-1:0] mid;
    genvar i;
    generate
        for(i = 0; i < WIDTH; i = i + 1) begin:edge_gen
            always @(posedge clk) begin
                mid[i] <= signal_in[i];
            end
            
            assign edge_detect_pulse[i] = (~mid[i]) & signal_in[i];
            
        end
    endgenerate

endmodule

