module synchronizer #(parameter WIDTH = 1) (
    input [WIDTH-1:0] async_signal,
    input clk,
    output [WIDTH-1:0] sync_signal
);
    // Simple 2-FF synchronizer

    reg [WIDTH-1:0] mid;
    reg [WIDTH-1:0] mid_2;
    genvar i;
    generate
        for(i = 0; i < WIDTH; i = i + 1) begin:synchronizer_gen
            always @(posedge clk) begin
                mid[i] <= async_signal[i];
                mid_2[i] <= mid[i];
            end
            
            assign sync_signal[i] = mid_2[i];
            
        end
    endgenerate


endmodule
