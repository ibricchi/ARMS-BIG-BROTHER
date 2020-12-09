//An ADD ALU situated at the top of the diagram
module div_mult_reg(
    input logic clk,
    input logic reset,

    input logic write_en,
    input logic[1:0] op,
    input logic[31:0] in_1,
    input logic[31:0] in_2,

    output logic[31:0] hi,
    output logic[31:0] lo
);

logic[63:0] prod;
assign prod = in_1 * in_2;

always_ff @(posedge clk) begin
    if(reset) begin
        hi <= 0;
        lo <= 0;
    end
    else begin 
        case (op)
            2'b00: begin // MTHI
                hi <= in_1;
            end
            2'b01: begin // MTLO
                lo <= in_2;
            end
            2'b10: begin // MULTU
                hi <= prod[63:32];
                lo <= prod[31:0];
            end
            2'b11: begin // DIVU
                lo <= in_1/in_2;
                hi <= in_1%in_2;
            end
        endcase
    end
end

endmodule