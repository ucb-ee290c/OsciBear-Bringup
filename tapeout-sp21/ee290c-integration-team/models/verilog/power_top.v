module power_top(
    input Vbat, //input 
    input Vref, //input
    input Iref, //input
    input Vss,    //input
    output VDDA, //Output
    output VDDD, //Output
    inout En,  //input
    inout pll_cref,
    inout lo_cref,
    inout div_cref,
    inout bp_cref_i,
    inout bp_cref_q,
    inout vga_cref_0i,
    inout vga_cref_1i,
    inout vga_cref_0q,
    inout vga_cref_1q,
    inout mixer_cref,
    inout buff_cref_i,
    inout buff_cref_q,
    inout test_cref
    );   
endmodule 
