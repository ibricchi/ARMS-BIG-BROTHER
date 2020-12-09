//An ADD ALU situated at the top of the diagram
module div_mult_reg(
    input logic clk;
    input logic write_en;
    input logic[1:0] op;
    input logic[31:0] in_1;
    input logic[31:0] in_2;
    input logic reg_to_reg;

    output logic[31:0] lo;
    output logic[31:0] hi;
);

logic[63:0] prod;
assign prod = in_1 * in_2;

always_ff @(posedge clk) begin
    case (op)
        2'b00: // MTHI
            lo <= in_1;
        2'b01: // MTLO
            hi <= in_2;
        2'b10: // MULTU
            hi <= prod[63:32];
            lo <= prod[31:0];
        2'b11: // DIVU
            lo <= in_1/in_2;
            hi <= in_1%in_2;

    endcase
end

endmodule