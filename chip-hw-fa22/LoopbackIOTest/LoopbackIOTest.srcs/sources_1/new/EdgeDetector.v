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
    reg [1:0] data;
    assign falling = data[0] & !data[1];
    assign rising = !data[0] & data[1];
    always @(posedge clk) begin
        data <= {data[0], D}; 
    end

endmodule
