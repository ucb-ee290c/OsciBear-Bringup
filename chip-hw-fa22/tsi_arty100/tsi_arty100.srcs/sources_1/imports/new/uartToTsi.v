module uartToTsi #(
    parameter SYSCLK = 100_000_000,
    parameter WIDTH = 123,
    parameter BAUD = 2_000_000
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
    //ila_3(.clk(clk), .probe0(packet_counter_Q), .probe1(countdown_Q), .probe2(uart_fire), .probe3(uart_fall), .probe4(uart_idle), .probe5(uart_rx));
    localparam 
        BAUD_CYCLE = SYSCLK / BAUD,
        BYTE_WIDTH = (WIDTH-1)/8 + 1;

    wire [3-1:0] packet_counter_D, packet_counter_Q;
    wire [16-1:0] countdown_Q;

    wire uart_fire, uart_fall, uart_idle;
    assign uart_fire =  (packet_counter_Q == 3'd0) ? (countdown_Q == BAUD_CYCLE*3/2) : 
                        (countdown_Q == BAUD_CYCLE);

    assign uart_idle = (packet_counter_Q == 3'd0) & (!uart_fall && (countdown_Q == 16'd0));
    
    // latch to make sure signals hold for one whole cycle
    /*
    always @(tl_rising_clk) begin
        tl_out_data <= rst ? 1'b0 : sreg_out;
        tl_out_valid <= rst ? 1'b0 : sreg_mode;
    end
    */
    
    RegInit #(
        .WIDTH(3), 
        .INIT(0))
    packet_counter(
        .clk(clk), 
        .rst(rst || uart_idle), 
        .en(uart_fire),
        .D(packet_counter_Q + 3'b001),
        .Q(packet_counter_Q)
    );
    RegInit #(
        .WIDTH(16), 
        .INIT(0)) 
    countdown(
        .clk(clk), 
        .rst(rst || uart_fire || uart_idle), 
        .en(1),
        .D(countdown_Q + 1'b1),
        .Q(countdown_Q)
    );
    shiftReg #(
        .IN_WIDTH(BYTE_WIDTH*8), 
        .OUT_WIDTH(WIDTH), 
        .D_WIDTH(BYTE_WIDTH*8)) 
    sreg0(
        .clk(clk), 
        .rst(rst), 
        .en(tl_out_valid ? (tl_rising_clk & tl_out_rd) : uart_fire), 
        .D(uart_rx),

        .mode(tl_out_valid), // 0: shifting in, 1: shifting out
        .Q(tl_out_data),
        .RESET(RESET)
    );
    EdgeDetector uartEdge (
        .clk(clk),
        .in(uart_rx),
        //.rising(),
        .falling(uart_fall)
    );

endmodule