module edge_detector #(
    parameter WIDTH = 1
)(
    input clk,
    input [WIDTH-1:0] signal_in,
    output [WIDTH-1:0] edge_detect_pulse
);
    // TODO: implement a multi-bit edge detector that detects a rising edge of 'signal_in[x]'
    // and outputs a one-cycle pulse 'edge_detect_pulse[x]' at the next clock edge
    // Feel free to use as many number of registers you like

    // Remove this line once you create your edge detector
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

