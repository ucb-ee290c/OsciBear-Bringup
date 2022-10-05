module shiftReg #(
    parameter IN_WIDTH = 123,
    parameter OUT_WIDTH = 128,
    parameter D_WIDTH = 128
    parameter INTERRUPT = 128{1'b1}
) (
    input clk,
    input rst, 
    input en, // when high shifts signal, mask low if not ready
    input D,

    output mode, // 0: shifting in, 1: shifting out
    output Q,
    output RESET
);
    localparam LOG_WIDTH = $clog2(D_WIDTH);
    reg [D_WIDTH-1:0] data;
    wire [LOG_WIDTH-1:0] counter_Q;
    assign Q = data[counter_Q];
    assign mode = mode_Q;
    assign RESET = INTERRUPT == data;

    reg modeFlip;
    assign modeFlip = mode_Q ? (counter_Q == OUT_WIDTH-1) : (counter_Q == IN_WIDTH-1);

    RegInit mode #(.WIDTH(1), .INIT(0)) (
        .clk(clk),
        .rst(rst),
        .en(en || modeFlip),
        .D(modeFlip ? !mode : mode),
        .Q(mode)
    );
    RegInit counter #(.WIDTH(LOGWIDTH-1), .INIT(0)) (
        .clk(clk),
        .rst(rst || modeFlip),
        .en(en),
        .D(counter_Q + 1'b1),
        .Q(counter_Q)
    );

    always @(posedge clk) begin
        data[counter_Q] <= D;
    end

endmodule
