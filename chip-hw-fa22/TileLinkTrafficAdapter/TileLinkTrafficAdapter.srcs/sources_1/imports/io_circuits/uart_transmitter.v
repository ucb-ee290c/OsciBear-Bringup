module uart_transmitter #(
    parameter CLOCK_FREQ = 125_000_000,
    parameter BAUD_RATE = 115_200)
(
    input clk,
    input reset,

    input [7:0] data_in,
    input data_in_valid,
    output reg data_in_ready,

    output reg serial_out
);
    // See diagram in the lab guide
    localparam  SYMBOL_EDGE_TIME    =   CLOCK_FREQ / BAUD_RATE;
    localparam  CLOCK_COUNTER_WIDTH =   $clog2(SYMBOL_EDGE_TIME);

    localparam IDLE = 0, START = 1, DATA = 2, STOP = 3;

    reg [CLOCK_COUNTER_WIDTH-1:0] baud_counter;
    reg [3:0] word_counter;
    reg transmit_phase;
    reg [7:0] tx_buffer;

    always @ (posedge clk) begin
        if (reset) begin
            baud_counter <= 0;
            word_counter <= 1;
            transmit_phase  <= 0;
            serial_out <= 1;
            data_in_ready  <= 1;
            tx_buffer  <= 8'b0;
        end
        else if(!transmit_phase && data_in_valid) begin
            // Start bit phase
            data_in_ready <= 0;
            serial_out <= 0;
            word_counter  <= 0;
            baud_counter <= 0;
            transmit_phase  <= 1;
            tx_buffer  <= data_in;
        end
        else if (transmit_phase && baud_counter >= SYMBOL_EDGE_TIME - 1) begin
            // On baud clock during transmit phase
            baud_counter <= 0;
            if(word_counter >= 0 && word_counter < 8) begin
                // Data transmit phase
                data_in_ready  <= 0;
                serial_out <= tx_buffer >> word_counter;
                word_counter <= word_counter + 1;
                transmit_phase  <= 1;
            end
            else if(word_counter == 8) begin
                // Stop bit phase
                data_in_ready <= 0;
                serial_out <= 1; // Stop bit
                word_counter <= word_counter + 1;
                transmit_phase  <= 1;
            end
            else begin
                // Return to idle
                data_in_ready <= 1;
                serial_out <= 1;
                tx_buffer  <= 8'b0;
                word_counter <= 0;
                transmit_phase  <= 0;
            end
        end
        else begin
            if(transmit_phase) begin
                baud_counter <= baud_counter + 1;
            end
            else begin
                baud_counter <= 0;
            end
        end
    end
endmodule
