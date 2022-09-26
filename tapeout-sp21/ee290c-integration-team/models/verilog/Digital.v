module Digital (
        input VDD, input VSS,
        input  jtag_TCK,                   //   # PADS
        input  jtag_TMS,                   //   
        input  jtag_TDI,                   //   
        output jtag_TDO_data,         //   
        output jtag_TDO_data_driven,//   

        output serial_tl_clock,              //   # PADS
        output serial_tl_bits_in_ready,   //  
        input  serial_tl_bits_in_valid,   //  
        input  serial_tl_bits_in_bits,    //  
        input  serial_tl_bits_out_ready, //  
        output serial_tl_bits_out_valid, //  
        output serial_tl_bits_out_bits,  //  

        output        baseband_offChipMode_rx, // # PADS
        output        baseband_offChipMode_tx, // # PADS
        output [7:0]  baseband_data_tx_loFSK,           // # DTOP      # AAA
        input  [7:0]  baseband_data_rx_i_data,           // # RF
        input  [7:0]  baseband_data_rx_q_data,           // # RF
        output [7:0]  baseband_data_loCT,            // # DTOP      # AAA
        output [10:0] baseband_data_pllD,                   // # DTOP      # AAA
        
        output [7:0] baseband_tuning_trim_g0,       //  #
        output [7:0] baseband_tuning_trim_g1,       //  #
        output [7:0] baseband_tuning_trim_g2,       //  #
        output [7:0] baseband_tuning_trim_g3,       //  #
        output [7:0] baseband_tuning_trim_g4,       //  #
        output [7:0] baseband_tuning_trim_g5,       //  #
        output [7:0] baseband_tuning_trim_g6,       //  #
        output [7:0] baseband_tuning_trim_g7,       //  #
         
        output [3:0] baseband_tuning_mixer_r0,       //  #
        output [3:0] baseband_tuning_mixer_r1,       //  #
        output [3:0] baseband_tuning_mixer_r2,       //  #
        output [3:0] baseband_tuning_mixer_r3,       //  #
        output [9:0] baseband_tuning_i_vgaAtten,     //  #
        output [3:0] baseband_tuning_i_filter_r0,         //  #
        output [3:0] baseband_tuning_i_filter_r1,         //  #
        output [3:0] baseband_tuning_i_filter_r2,         //  #
        output [3:0] baseband_tuning_i_filter_r3,         //  #
        output [3:0] baseband_tuning_i_filter_r4,         //  #
        output [3:0] baseband_tuning_i_filter_r5,         //  #
        output [3:0] baseband_tuning_i_filter_r6,         //  #
        output [3:0] baseband_tuning_i_filter_r7,         //  #
        output [3:0] baseband_tuning_i_filter_r8,         //  #
        output [3:0] baseband_tuning_i_filter_r9,         //  #
        output [9:0] baseband_tuning_q_vgaAtten,     //  #
        output [3:0] baseband_tuning_q_filter_r0,         //  #
        output [3:0] baseband_tuning_q_filter_r1,         //  #
        output [3:0] baseband_tuning_q_filter_r2,         //  #
        output [3:0] baseband_tuning_q_filter_r3,         //  #
        output [3:0] baseband_tuning_q_filter_r4,         //  #
        output [3:0] baseband_tuning_q_filter_r5,         //  #
        output [3:0] baseband_tuning_q_filter_r6,         //  #
        output [3:0] baseband_tuning_q_filter_r7,         //  #
        output [3:0] baseband_tuning_q_filter_r8,         //  #
        output [3:0] baseband_tuning_q_filter_r9,         //  #
        output [5:0] baseband_tuning_dac_t0,     //  #
        output [5:0] baseband_tuning_dac_t1,     //  #
        output [5:0] baseband_tuning_dac_t2,     //  #
        output [5:0] baseband_tuning_dac_t3,     //  #
        output [4:0] baseband_tuning_enable_rx_i,      //  #
        output [4:0] baseband_tuning_enable_rx_q,      //  #
        output [9:0] baseband_tuning_mux_dbg_in,     //  #
        output [9:0] baseband_tuning_mux_dbg_out,   //  #

        input  gpio_0_0_i,     //  #
        output gpio_0_0_o,     //  #
        output gpio_0_0_ie,                //  #
        output gpio_0_0_oe,  //  #

        input  gpio_0_1_i,   // #
        output gpio_0_1_o,   // #
        output gpio_0_1_ie,              // #
        output gpio_0_1_oe,// #

        input  gpio_0_2_i,   // #
        output gpio_0_2_o,   // #
        output gpio_0_2_ie,             //  #
        output gpio_0_2_oe,// #

        output spi_0_sck,        // output #
        output spi_0_cs_0,        // output #

        input  spi_0_dq_0_i,    // #
        output spi_0_dq_0_o,    // #
        output spi_0_dq_0_oe, // #

        input  spi_0_dq_1_i,    // #
        output spi_0_dq_1_o,    // #
        output spi_0_dq_1_oe, // #

        input  spi_0_dq_2_i,     // #
        output spi_0_dq_2_o,     // #
        output spi_0_dq_2_oe,  // #

        input  spi_0_dq_3_i,     // #
        output spi_0_dq_3_o,     // #
        output spi_0_dq_3_oe,  // #

        output uart_0_txd,      // #
        input  uart_0_rxd,      // #
        input  bsel,               // #
        input  reset_wire_reset,  // #
        input  clock        // #
    );

endmodule
