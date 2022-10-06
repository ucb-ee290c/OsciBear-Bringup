module shiftReg #(
    parameter IN_WIDTH = 123,
    parameter OUT_WIDTH = 128,
    parameter D_WIDTH = 128,
    parameter INTERRUPT = {128{1'b1}}
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
    wire mode_Q;
    assign Q = data[counter_Q];
    assign mode = mode_Q;
    assign RESET = INTERRUPT == data;
    
    ila_1(  .clk(clk), .probe0(data), .probe1(counter_Q), .probe2(mode),
            .probe3(rst), .probe4(en), .probe5(D), .probe6(Q));
    
    wire modeFlip;
    assign modeFlip = mode_Q ? (counter_Q == OUT_WIDTH-1) : (counter_Q == IN_WIDTH-1);

    RegInit #(.WIDTH(1), .INIT(0)) 
    mode(
        .clk(clk),
        .rst(rst),
        .en(en || modeFlip),
        .D(modeFlip ? !mode : mode),
        .Q(mode_Q)
    );
    RegInit #(.WIDTH(LOG_WIDTH-1), .INIT(0)) 
    counter(
        .clk(clk),
        .rst(rst || modeFlip),
        .en(en),
        .D(counter_Q + 1'b1),
        .Q(counter_Q)
    );

    always @(posedge clk) begin
        // this is counterintuitive: when enable is high, we want the data to be frozen for 1 cycle before
        data[counter_Q] <= en ? data[counter_Q] : D;
    end

endmodule
