module tsiToUart #(
    parameter SYSCLK = 100_000_000,
    parameter WIDTH = 123,
    parameter BAUD = 2_000_000
) (
    input clk,
    input rst,
    output uart_tx,
    input tl_rising_clk,
    output tl_in_rd,
    input tl_in_data,
    input tl_in_valid
);
    localparam BAUD_CYCLE = SYSCLK / BAUD;
    localparam BYTE_WIDTH = (WIDTH-1)/8 + 1;
    wire [3:0] packet_counter_Q;
    wire [16-1:0] countdown_Q;
    wire uart_fire, uart_idle_Q, sreg_isOut, sreg_out;
    assign uart_tx = (packet_counter_Q == 0) ? 1'b0 : ((packet_counter_Q == 9) ? 1'b1 : sreg_out);
    assign uart_fire = (countdown_Q == 0);
    assign tl_in_rd = !sreg_isOut;

    RegInit #(.WIDTH(1), .INIT(1)) 
    uart_idle(
        .clk(clk), .rst(rst), .en(1),
        .D((packet_counter_Q == 0) ? sreg_isOut : (packet_counter_Q == 9)),
        .Q(uart_idle_Q)
    );
    RegInit #(.WIDTH(3), .INIT(0)) 
    packet_counter(
        .clk(clk), .rst(rst || sreg_isOut), .en(uart_fire),
        .D(packet_counter_Q + 1'b1),
        .Q(packet_counter_Q)
    );
    RegInit #(.WIDTH(16), .INIT(BAUD_CYCLE)) 
    countdown(
        .clk(clk), .rst(rst || sreg_isOut), .en(1),
        .D(countdown_Q - 1'b1),
        .Q(countdown_Q)
    );
    shiftReg #(.IN_WIDTH(WIDTH), .OUT_WIDTH(BYTE_WIDTH*8), .D_WIDTH(BYTE_WIDTH*8)) 
    sreg0(
        .clk(clk), .rst(rst), 
        .en(sreg_isOut ? !((packet_counter_Q == 0) || (packet_counter_Q == 9)) : (tl_rising_clk & tl_in_valid)), 
        .D(tl_in_data),

        .mode(sreg_isOut), // 0: shifting in, 1: shifting out
        .Q(sreg_out)
    );

endmodule