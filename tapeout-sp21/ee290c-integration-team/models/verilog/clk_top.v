/* DEPRECATED */
module clk_top(
	input VDD_A,
	input CLK_OVER,
	input cpu_ref,
	input rf_ref,
	output RF_CLK_OUT,
	output CPU_CLK_OUT
);

    reg CPU_CLK_OUT_reg, RF_CLK_OUT_reg, temp_A, temp_D;
/* 
    //Mux for CPU clk
    always @(posedge cpu_ref) begin
      case(CLK_OVER) 
        1'b0 : begin //Double inverter for now as we are unsure how to model PLL in verilog
	  temp_D <= ~cpu_ref;
	  CPU_CLK_OUT_reg <= ~temp_D;
        end
        1'b1 : begin
	  CPU_CLK_OUT_reg <= cpu_ref;
        end
      endcase  
    end
*/
    //Mux for RF clk
    always @(posedge rf_ref) begin 
      case(CLK_OVER) 
        1'b0 : begin //Double inverter for now as we are unsure how to model PLL in verilog
	  temp_A <= ~rf_ref;
	  RF_CLK_OUT_reg <= ~temp_A;
        end
        1'b1 : begin
	  RF_CLK_OUT_reg <= rf_ref;
        end
      endcase  
    end

assign CPU_CLK_OUT = cpu_ref;
assign RF_CLK_OUT = rf_ref; //RF_CLK_OUT_reg;

endmodule 
