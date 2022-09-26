module analog_top(
        input VDD,
        input VSS,
        input ref_clk,
        input cpu_clk,
        
        input pll_cref,
        input lo_cref,
        input div_cref,
        input bp_cref_i,
        input bp_cref_q,
        input vga_cref_0i,
        input vga_cref_1i,
        input vga_cref_0q,
        input vga_cref_1q,
        input mixer_cref,
        input buff_cref_i,
        input buff_cref_q,
        input test_cref,

        inout Analog_Test_1,
        inout Analog_Test_2,
        inout Analog_Test_3,
        inout Analog_Test_4,
        input test_enable,
        input [9:0] mux_dbg_in,
        output [9:0] mux_dbg_out,
        
	//Set bits when we know ADC_WIDTH
        output [7:0] i_adc_code,             
        output [7:0] q_adc_code, 
        input i_adc_data_valid,
        input q_adc_data_valid,
        input [4:0] i_enable,
        input [4:0] q_enable,
        
        output tx,
	    input rx,
	    
        input [7:0] lo_fsk_tune, 
        input [7:0] lo_coarse_tune,
        input [10:0] pll_div,
        output pll_ready,
        input lo_enable,
        
	//set bits when we know DAC_WIDTH
        input f_enable_i,
        input [3:0] filter_res0i,
        input [3:0] filter_res1i, 
        input [3:0] filter_res2i,
        input [3:0] filter_res3i,
        input [3:0] filter_res4i,
        input [3:0] filter_res5i,
        input [3:0] filter_res6i,
        input [3:0] filter_res7i,
        input [3:0] filter_res8i,
        input [3:0] filter_res9i,
        
        input f_enable_q,
        input [3:0] filter_res0q,
        input [3:0] filter_res1q, 
        input [3:0] filter_res2q,
        input [3:0] filter_res3q,
        input [3:0] filter_res4q,
        input [3:0] filter_res5q,
        input [3:0] filter_res6q,
        input [3:0] filter_res7q,
        input [3:0] filter_res8q,
        input [3:0] filter_res9q,
        
        input vga_enable_0i,
        input vga_enable_1i,
        input vga_enable_0q,
        input vga_enable_1q,
        input [4:0] VGA_atteni, 
        input [4:0] VGA_attenq,
        input [5:0] current_dac_0i,
        input [5:0] current_dac_1i,
        input [5:0] current_dac_0q,
        input [5:0] current_dac_1q
        
        input [3:0] mix_res_in,
        input [3:0] mix_res_ip,
        input [3:0] mix_res_qn,
        input [3:0] mix_res_qp
        input mix_enable_i,
        input mix_enable_q,
        
        input buffer_enable_i,
        input buffer_enable_q
    );



endmodule
