module test(
);
    wire middle, VDD, VSS;
    INV inv1(.VDD(VDD), .VSS(VSS), .ZN(middle));
    INV inv2(.VDD(VDD), .VSS(VSS), .I(middle));
endmodule

