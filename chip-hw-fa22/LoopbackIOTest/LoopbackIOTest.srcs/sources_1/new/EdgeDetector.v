module EdgeDetector (
    input clk,
    input in,
    output rising,
    output falling
);
    /**
     * Design assumptions: D is an offchip signal, there are no timing guarentees
     * 1. posedge clk, store last, compare current signal. Pros: no cycle lost, Cons: potential meta-stable state when D arrives very closely before clk. 
     * => 2. posedge clk, store last 2, output if last 2 dif. Pros: no meta-stability, Cons: 1 lost cycle need to be accounted. But not sampling/sending immediately anyways. 
     * 3. Use negedge clk, store last, compare current signal. Not sure how fpga will like this...
     */
    /*
    wire [1:0] data_Q;
    
    assign falling = data_Q[1] & !data_Q[0];
    assign rising = !data_Q[1] & data_Q[0];
    //assign falling = data_Q[0];
    //assign rising = data_Q[1];
    
    RegInit #(.WIDTH(2), .INIT(0))
    data(
        .clk(clk), .rst(rst), .en(1),
        .D({data_Q[0], in}),
        .Q(data_Q)
    );
    */
    
    
    reg data;
    always @(posedge clk) begin
        data <= in;
    end
    
    assign rising = (~data) & in;
    assign falling = data & ~in;
    

endmodule
