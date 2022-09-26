module core (
    output [2:0] gpio_wr_enn,
    output [2:0] gpio_wr_d,
    input  [2:0] gpio_rd_d,
// RF I/O Pins
    input rf_rx,
    output rf_tx,
    inout [4:1] analog_test, 
// CPU I/O Pins,
    output uart_tx,
    input uart_rx,
    input msel,
    input jtag_tck,
    output jtag_tdo_data,
    output jtag_tdo_driven,
    input jtag_tms,
    input jtag_tdi,
    output spi_cs,
    output spi_sck,
    output [3:0] spi_dq_wr_enn,
    output [3:0] spi_dq_wr_d,
    input  [3:0] spi_dq_rd_d,
    output tl_clk,
    input tl_in_dat,
    output tl_in_rdy,
    input tl_in_val,
    output tl_out_dat,
    input tl_out_rdy,
    output tl_out_val,
//DBB I/O Pin,
    output dbb_off_chip_mode_tx,
    output dbb_off_chip_mode_rx,
// Power/Clocking I/O Pins,
    input reset,
    input cpu_ref_clk,
    input rf_ref_clk,
    input v_bat,
    input v_ref,
    input i_ref,
    input gnd,
    input io_pwr,
    input ldo_en,
    input vdd_d,
    input vdd_a

);

    //Intrachip Connections: // source destinations

    // LO # CHECKED: RF.disc(fsk=3:0 coarse=4:0)
    wire [7:0] lo_fsk_tune; // DTOP RF
    wire [7:0] lo_coarse_tune; // DTOP RF // matcing pins
    wire [10:0] pll_div; // DTOP RF
    
    // ADC # CHECKED: DTOP.disc(ADC VALID SIGNALS MISSING)
    wire i_adc_data_valid, q_adc_data_valid; // RF DTOP
    wire [7:0] i_adc_code, q_adc_code; // RF DTOP
    
    // FILTER # CHECKED
    wire [3:0] f_r0i, f_r1i, f_r2i, f_r3i, f_r4i, f_r5i, f_r6i, f_r7i, f_r8i, f_r9i; // DTOP RF
    wire [3:0] f_r0q, f_r1q, f_r2q, f_r3q, f_r4q, f_r5q, f_r6q, f_r7q, f_r8q, f_r9q; // DTOP RF
    
    // VGA # CHECKED
    wire [9:0] vga_attenq, vga_atteni; // DTOP RF
    wire [5:0] current_dac_0i, current_dac_1i, current_dac_0q, current_dac_1q; // DTOP RF
    
    // ENABLES # CHECKED {bpf, buf, mix, vga_1, vga_0}
    wire [4:0] i_enable, q_enable; // DTOP RF
    
    // MIXER # CHECKED
    wire [3:0] mix_res_in, mix_res_ip, mix_res_qn, mix_res_qp; // DTOP RF
    
    // REFERENCES
    wire pll_cref, lo_cref, div_cref, bp_cref_i, bp_cref_q, vga_cref_0i, vga_cref_1i, vga_cref_0q, vga_cref_1q, mixer_cref; // PWR RF
    wire buff_cref_i, buff_cref_q, test_cref; // PWR RF
    
    //Mux Dbg pins # CHECKED
    wire [9:0] mux_dbg_in, mux_dbg_out; // DTOP RF
    
    //Etc Trim bits # CHECKED
    wire [7:0] tuning_trim_g0, tuning_trim_g1, tuning_trim_g2, tuning_trim_g3, tuning_trim_g4, tuning_trim_g5, tuning_trim_g6, tuning_trim_g7; // DTOP RF
    
    power_top PWR(
        .Vbat(v_bat), //input 
        .Vref(v_ref), //input
        .Iref(i_ref), //input
        .Vss(gnd),    //input
        .VDDA(vdd_a), //Output
        .VDDD(vdd_d), //Output
        .En(ldo_en),  //input
        //Current References
        .pll_cref(pll_cref),
        .lo_cref(lo_cref),
        .div_cref(div_cref),
        .bp_cref_i(bp_cref_i),
        .bp_cref_q(bp_cref_q),
        .vga_cref_0i(vga_cref_0i),
        .vga_cref_1i(vga_cref_1i),
        .vga_cref_0q(vga_cref_0q),
        .vga_cref_1q(vga_cref_1q),
        .mixer_cref(mixer_cref),
        .buff_cref_i(buff_cref_i),
        .buff_cref_q(buff_cref_q),
        .test_cref(test_cref)
    );   

    rf_top_with_LO_v10 RF(
        .VDDA(vdd_a),
        .VSS(gnd),
        .rf_ref_clk(rf_ref_clk),
        .cpu_clk(cpu_ref_clk),

        //Current References
        .pll_cref(pll_cref),
        .lo_cref(lo_cref),
        .div_cref(div_cref),
        .bp_cref_i(bp_cref_i),
        .bp_cref_q(bp_cref_q),
        .vga_cref_0i(vga_cref_0i),
        .vga_cref_1i(vga_cref_1i),
        .vga_cref_0q(vga_cref_0q),
        .vga_cref_1q(vga_cref_1q),
        .mixer_cref(mixer_cref),
        .buff_cref_i(buff_cref_i),
        .buff_cref_q(buff_cref_q),
        .test_cref(test_cref),
        
        //testing I/O pins
        .analog_test1(analog_test[1]),
        .analog_test2(analog_test[2]),
        .analog_test3(analog_test[3]),
        .analog_test4(analog_test[4]),
        .test_en(tuning_trim_g7[0]),
        .rf_clk_sel(tuning_trim_g0[5]),
        .analog_test2_sel(tuning_trim_g0[4]),
        .mux_dbg_in(mux_dbg_in),
        .mux_dbg_out(mux_dbg_out),
       

        //signal chain (ADC)
        .i_adc_code(i_adc_code),     
        .q_adc_code(q_adc_code), 
        .i_adc_data_valid(i_adc_data_valid),
        .q_adc_data_valid(q_adc_data_valid),
        .i_enable(i_enable),
        .q_enable(q_enable),
        
        //Tx and Rx
        .RF_TX(rf_tx),
        .RF_IN(rf_rx),

        //LO control
        .lo_fsk_tune(lo_fsk_tune),
        .lo_coarse_tune(lo_coarse_tune[5:0]),
        .pll_div(pll_div),
        .cc_enable(tuning_trim_g0[1]), 
        .ref_sel(tuning_trim_g0[2]), 
        .pll_sign(tuning_trim_g0[3]), 
        .pll_idac(tuning_trim_g1[5:0]),
        .lo_enable(tuning_trim_g0[0]),

        //Filter
        .filter_res0i(f_r0i),
        .filter_res1i(f_r1i), 
        .filter_res2i(f_r2i),
        .filter_res3i(f_r3i),
        .filter_res4i(f_r4i),
        .filter_res5i(f_r5i),
        .filter_res6i(f_r6i),
        .filter_res7i(f_r7i),
        .filter_res8i(f_r8i),
        .filter_res9i(f_r9i),
        
        .filter_res0q(f_r0q),
        .filter_res1q(f_r1q), 
        .filter_res2q(f_r2q),
        .filter_res3q(f_r3q),
        .filter_res4q(f_r4q),
        .filter_res5q(f_r5q),
        .filter_res6q(f_r6q),
        .filter_res7q(f_r7q),
        .filter_res8q(f_r8q),
        .filter_res9q(f_r9q),
        
        .VGA_atteni(vga_atteni), 
        .VGA_attenq(vga_attenq), 
        .current_dac_0i(current_dac_0i),
        .current_dac_1i(current_dac_1i),
        .current_dac_0q(current_dac_0q),
        .current_dac_1q(current_dac_1q),
        
        //Mixer
        .mix_res_in(mix_res_in),
        .mix_res_ip(mix_res_ip),
        .mix_res_qn(mix_res_qn),
        .mix_res_qp(mix_res_qp)
    );
    
    Digital DTOP( // @[chipyard.TestHarness.BasebandRocketConfig.fir 304649:2]
        .VDD(vdd_d),
        .VSS(gnd),
        .jtag_TCK(jtag_tck),                   // input   # PADS
        .jtag_TMS(jtag_tms),                   // input   
        .jtag_TDI(jtag_tdi),                   // input   
        .jtag_TDO_data(jtag_tdo_data),         // output  
        .jtag_TDO_data_driven(jtag_tdo_driven),// output  

        .serial_tl_clock(tl_clk),              // output   # PADS
        .serial_tl_bits_in_ready(tl_in_rdy),   // output  
        .serial_tl_bits_in_valid(tl_in_val),   // input   
        .serial_tl_bits_in_bits(tl_in_dat),    // input   
        .serial_tl_bits_out_ready(tl_out_rdy), // input   
        .serial_tl_bits_out_valid(tl_out_val), // output  
        .serial_tl_bits_out_bits(tl_out_dat),  // output  

        .baseband_offChipMode_rx(dbb_off_chip_mode_rx), // output        # PADS
        .baseband_offChipMode_tx(dbb_off_chip_mode_tx), // output        # PADS
        .baseband_data_tx_loFSK(lo_fsk_tune),           // output [7:0]  # DTOP      # AAA
        .baseband_data_rx_i_data(i_adc_code),           // input  [7:0]  # RF
        .baseband_data_rx_q_data(q_adc_code),           // input  [7:0]  # RF
        .baseband_data_loCT(lo_coarse_tune),            // output [7:0]  # DTOP      # AAA
        .baseband_data_pllD(pll_div),                   // output [10:0] # DTOP      # AAA
        
        .baseband_tuning_trim_g0(tuning_trim_g0),       // output [7:0]  #
        .baseband_tuning_trim_g1(tuning_trim_g1),       // output [7:0]  #
        .baseband_tuning_trim_g2(tuning_trim_g2),       // output [7:0]  #
        .baseband_tuning_trim_g3(tuning_trim_g3),       // output [7:0]  #
        .baseband_tuning_trim_g4(tuning_trim_g4),       // output [7:0]  #
        .baseband_tuning_trim_g5(tuning_trim_g5),       // output [7:0]  #
        .baseband_tuning_trim_g6(tuning_trim_g6),       // output [7:0]  #
        .baseband_tuning_trim_g7(tuning_trim_g7),       // output [7:0]  #
         
        .baseband_tuning_mixer_r0(mix_res_in),       // output [3:0]  #
        .baseband_tuning_mixer_r1(mix_res_ip),       // output [3:0]  #
        .baseband_tuning_mixer_r2(mix_res_qn),       // output [3:0]  #
        .baseband_tuning_mixer_r3(mix_res_qp),       // output [3:0]  #
        .baseband_tuning_i_vgaAtten(vga_atteni),     // output [9:0]  #
        .baseband_tuning_i_filter_r0(f_r0i),         // output [3:0]  #
        .baseband_tuning_i_filter_r1(f_r1i),         // output [3:0]  #
        .baseband_tuning_i_filter_r2(f_r2i),         // output [3:0]  #
        .baseband_tuning_i_filter_r3(f_r3i),         // output [3:0]  #
        .baseband_tuning_i_filter_r4(f_r4i),         // output [3:0]  #
        .baseband_tuning_i_filter_r5(f_r5i),         // output [3:0]  #
        .baseband_tuning_i_filter_r6(f_r6i),         // output [3:0]  #
        .baseband_tuning_i_filter_r7(f_r7i),         // output [3:0]  #
        .baseband_tuning_i_filter_r8(f_r8i),         // output [3:0]  #
        .baseband_tuning_i_filter_r9(f_r9i),         // output [3:0]  #
        .baseband_tuning_q_vgaAtten(vga_attenq),     // output [9:0]  #
        .baseband_tuning_q_filter_r0(f_r0q),         // output [3:0]  #
        .baseband_tuning_q_filter_r1(f_r1q),         // output [3:0]  #
        .baseband_tuning_q_filter_r2(f_r2q),         // output [3:0]  #
        .baseband_tuning_q_filter_r3(f_r3q),         // output [3:0]  #
        .baseband_tuning_q_filter_r4(f_r4q),         // output [3:0]  #
        .baseband_tuning_q_filter_r5(f_r5q),         // output [3:0]  #
        .baseband_tuning_q_filter_r6(f_r6q),         // output [3:0]  #
        .baseband_tuning_q_filter_r7(f_r7q),         // output [3:0]  #
        .baseband_tuning_q_filter_r8(f_r8q),         // output [3:0]  #
        .baseband_tuning_q_filter_r9(f_r9q),         // output [3:0]  #
        .baseband_tuning_dac_t0(current_dac_0i),     // output [5:0]  #
        .baseband_tuning_dac_t1(current_dac_1i),     // output [5:0]  #
        .baseband_tuning_dac_t2(current_dac_0q),     // output [5:0]  #
        .baseband_tuning_dac_t3(current_dac_1q),     // output [5:0]  #
        .baseband_tuning_enable_rx_i(i_enable),      // output [4:0]  #
        .baseband_tuning_enable_rx_q(q_enable),      // output [4:0]  #
        .baseband_tuning_mux_dbg_in(mux_dbg_in),     // output [9:0]  #
        .baseband_tuning_mux_dbg_out(mux_dbg_out),   // output [9:0]  #

        .gpio_0_0_i(gpio_rd_d[0]),     // input  #
        .gpio_0_0_o(gpio_wr_d[0]),     // output #
        .gpio_0_0_ie(),               // output #
        .gpio_0_0_oe(gpio_wr_enn[0]),  // output #

        .gpio_0_1_i(gpio_rd_d[1]),   // input  #
        .gpio_0_1_o(gpio_wr_d[1]),   // output #
        .gpio_0_1_ie(),             // output #
        .gpio_0_1_oe(gpio_wr_enn[1]),  // output #

        .gpio_0_2_i(gpio_rd_d[2]),   // input  #
        .gpio_0_2_o(gpio_wr_d[2]),   // output #
        .gpio_0_2_ie(),             // output #
        .gpio_0_2_oe(gpio_wr_enn[2]),  // output #

        .spi_0_sck(spi_sck),        // output #
        .spi_0_cs_0(spi_cs),        // output #

        .spi_0_dq_0_i(spi_dq_rd_d[0]),    // input  #
        .spi_0_dq_0_o(spi_dq_wr_d[0]),    // output #
        .spi_0_dq_0_oe(spi_dq_wr_enn[0]), // output #

        .spi_0_dq_1_i(spi_dq_rd_d[1]),    // input  #
        .spi_0_dq_1_o(spi_dq_wr_d[1]),    // output #
        .spi_0_dq_1_oe(spi_dq_wr_enn[1]), // output #

        .spi_0_dq_2_i(spi_dq_rd_d[2]),     // input  #
        .spi_0_dq_2_o(spi_dq_wr_d[2]),     // output #
        .spi_0_dq_2_oe(spi_dq_wr_enn[2]),  // output #

        .spi_0_dq_3_i(spi_dq_rd_d[3]),     // input  #
        .spi_0_dq_3_o(spi_dq_wr_d[3]),     // output #
        .spi_0_dq_3_oe(spi_dq_wr_enn[3]),  // output #

        .uart_0_txd(uart_tx),      // output #
        .uart_0_rxd(uart_rx),      // input  #
        .bsel(msel),               // input  #
        .reset_wire_reset(reset),  // input  #
        .clock(cpu_ref_clk)        // input  #
    );

endmodule
