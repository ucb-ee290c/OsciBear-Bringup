module fifo #(
    parameter WIDTH = 8,
    parameter DEPTH = 32,
    parameter POINTER_WIDTH = $clog2(DEPTH)
) (
    input clk, rst,

    // Write side
    input wr_en,
    input [WIDTH-1:0] din,
    output reg full,

    // Read side
    input rd_en,
    output reg [WIDTH-1:0] dout,
    output reg empty
);

    
    reg [POINTER_WIDTH - 1:0] read_ptr;
    reg [POINTER_WIDTH - 1:0] write_ptr;
    
    reg [POINTER_WIDTH - 1:0] read_adv_ptr;
    reg [POINTER_WIDTH - 1:0] write_adv_ptr;
    
    reg  [WIDTH - 1:0] buffer [0:DEPTH - 1];
    
    always @(*) begin 
        if (wr_en && rd_en) begin
            dout = buffer[read_ptr];
        end
        else if (!wr_en && rd_en && !empty) begin
            dout = buffer[read_ptr];
        end
        else begin
            dout = 'b0;
        end 
    end
    
    always @ (posedge  clk) begin
        if (rst) begin
            read_ptr <= 0;
            write_ptr <= 0;
            empty <= 1;
            full <= 0;
            read_adv_ptr <= 'b1;
            write_adv_ptr <= 'b1;
        end
        else begin 
            if (wr_en && rd_en) begin
                buffer[write_ptr] <= din;
                write_ptr <= write_ptr + 1; 
                write_adv_ptr <= write_ptr + 2;
                read_ptr <= read_ptr + 1;
                read_adv_ptr <= read_ptr + 2;
                full <= full;
                empty <= empty;
            end
            else if (wr_en && !rd_en && !full) begin
                buffer[write_ptr] <= din;
                write_ptr <= write_ptr + 1;
                write_adv_ptr <= write_ptr + 2;
                full <= write_adv_ptr == read_ptr;
                empty <= 0;
            end
            else if (!wr_en && rd_en && !empty) begin
                read_ptr <= read_ptr + 1;
                read_adv_ptr <= read_ptr + 2; 
                empty <= read_adv_ptr == write_ptr;
                full <= 0;
            end 
        end
    end
endmodule
