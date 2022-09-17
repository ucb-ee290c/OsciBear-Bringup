module debouncer #(
    parameter WIDTH              = 1,
    parameter SAMPLE_CNT_MAX     = 62500,
    parameter PULSE_CNT_MAX      = 200,
    parameter WRAPPING_CNT_WIDTH = $clog2(SAMPLE_CNT_MAX),
    parameter SAT_CNT_WIDTH      = $clog2(PULSE_CNT_MAX) + 1
) (
    input clk,
    input [WIDTH-1:0] glitchy_signal,
    output [WIDTH-1:0] debounced_signal
);
    // TODO: fill in neccesary logic to implement the wrapping counter and the saturating counters
    // Some initial code has been provided to you, but feel free to change it however you like
    // One wrapping counter is required
    // One saturating counter is needed for each bit of glitchy_signal
    // You need to think of the conditions for reseting, clock enable, etc. those registers
    // Refer to the block diagram in the spec

    reg [SAT_CNT_WIDTH-1:0] saturating_counter [WIDTH-1:0];
    reg [WRAPPING_CNT_WIDTH-1:0] spg_counter;
    reg spg_out;



    // Sample Pulse Generator
    initial spg_counter = 0;
    always @(posedge clk) begin
        if(spg_counter == SAMPLE_CNT_MAX) begin
            spg_out <= 1'b1;
            spg_counter <= 'b0;
        end
        else begin
            spg_out <= 1'b0;
            spg_counter <= spg_counter + 1;
        end

    end

    genvar i;
    generate
        for(i = 0; i < WIDTH; i = i + 1) begin:debouncer_gen
            initial saturating_counter[i] = 0;
            always @(posedge clk) begin
                if(spg_out == 1'b1 && saturating_counter[i] != PULSE_CNT_MAX && glitchy_signal[i] == 1'b1) begin
                    saturating_counter[i] <= saturating_counter[i] + 1;
                end
                else if(glitchy_signal[i] == 0) begin
                    saturating_counter[i] <= 'b0;
                end
                else begin
                    saturating_counter[i] <= saturating_counter[i];
                end
            end

            assign debounced_signal[i] = saturating_counter[i] == PULSE_CNT_MAX & glitchy_signal[i] == 1'b1;
        end
    endgenerate


endmodule
