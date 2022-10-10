module uartToTsi #(
    parameter SYSCLK = 100_000_000,
    parameter WIDTH = 123,
    parameter BAUD = 2_000_000,
) (
    input clk,
    input rst,
    input uart_rx,
    input tl_rising_clk,
    input tl_out_rd,
    output tl_out_data,
    output tl_out_valid,
    output RESET
);
    localparam BAUD_CYCLE = SYSCLK / BAUD;
    localparam BYTE_WIDTH = (WIDTH-1)/8 + 1;
    wire [2:0] packet_counter_Q;
    wire [16-1:0] countdown_Q;
    wire [16-1:0]
    wire uart_fire, uart_fall, uart_idle_Q, sreg_isOut;
    assign uart_fire = (countdown_Q == 0);

    RegInit uart_idle #(.WIDTH(1), .INIT(1)) (
        .clk(clk), .rst(rst), .en(1),
        .D((packet_counter_Q == 0) ? !uart_fall : (packet_counter_Q == 7)),
        .Q(uart_idle_Q)
    );
    RegInit packet_counter #(.WIDTH(2), .INIT(0)) (
        .clk(clk), .rst(rst || uart_idle_Q), .en(uart_fire),
        .D(packet_counter_Q + 1'b1),
        .Q(packet_counter_Q)
    );
    RegInit countdown #(.WIDTH(16), .INIT(BAUD_CYCLE*3/2)) (
        .clk(clk), .rst(rst || uart_idle_Q), .en(1),
        .D(uart_fire ? BAUD_CYCLE : countdown_Q - 1'b1),
        .Q(countdown_Q)
    );
    shiftReg sreg0 #(.IN_WIDTH(BYTE_WIDTH*8), .OUT_WIDTH(WIDTH), .D_WIDTH(BYTE_WIDTH*8)) (
        .clk(clk), .rst(rst), 
        .en(sreg_isOut ? uart_fire : (tl_rising_clk & tl_out_rd)), 
        .D(uart_rx),

        .mode(tl_out_valid), // 0: shifting in, 1: shifting out
        .Q(tl_out_data)
        .RESET(RESET)
    );
    EdgeDetector uartEdge (
        .clk(clk),
        .in(uart_rx),
        //.rising(),
        .falling(uart_fall),
    );

endmodule