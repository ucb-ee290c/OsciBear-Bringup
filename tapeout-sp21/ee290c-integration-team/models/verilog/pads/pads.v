// model for PDU* including schmitt trigger version
module PDU(
    input REN, // active low pull up resistor enable
    input OEN, // active low output enable
    input  I,  // write data
    output C,  // read data
    inout PAD  // off-chip connection
);
always @(*) begin
    if !(OEN) begin // proper write
        PAD = I;
        C = PAD;
    end else begin 
        if   (PAD !== 1'bz) C = PAD; // pad externally driven
        else (PAD) begin             // unconnected pad
            if !(REN) begin          // safety pull up on
                C = 1'b1;
                PAD = 1'b1;
            end else begin           // safety pull up off
                C = 1'bx;
                PAD = 1'bz;
            end 
        end
    end
end
endmodule

module 

module PDD(
    input REN, // active low pull down resistor enable
    input OEN, // active low output enable
    input  I,  // write data
    output C,  // read data
    inout PAD  // off-chip connection
);
always @(*) begin
    if !(OEN) begin // proper write
        PAD = I;
        C = PAD;
    end else begin 
        if   (PAD !== 1'bz) C = PAD; // pad externally driven
        else (PAD) begin             // unconnected pad
            if !(REN) begin          // safety pull up on
                C = 1'b0;
                PAD = 1'b0;
            end else begin           // safety pull up off
                C = 1'bx;
                PAD = 1'bz;
            end 
        end
    end
end
endmodule


