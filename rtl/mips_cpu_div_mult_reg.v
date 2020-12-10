//An ADD ALU situated at the top of the diagram
module div_mult_reg(
    input logic clk,
    input logic reset,

    input logic write_en,
    input logic sin,
    input logic[1:0] op,
    input logic[31:0] in_1,
    input logic[31:0] in_2,

    output logic[31:0] hi,
    output logic[31:0] lo
);

logic[63:0] in_1_u;
logic[63:0] in_2_u;
logic[63:0] in_1_s;
logic[63:0] in_2_s;

logic[63:0] prod;
logic[63:0] div;
logic[63:0] rem;

always_comb begin
    in_1_u = $unsigned(in_1);
    in_2_u = $unsigned(in_2);
    in_1_s = $signed(in_1);
    in_2_s = $signed(in_2);
    prod = sin?(in_1_s*in_2_s):(in_1_u*in_2_u);
    div = sin?(in_1_s/in_2_s):(in_1_u/in_2_u);
    rem = sin?(in_1_s%in_2_s):(in_1_u%in_2_u);
end


always_ff @(posedge clk) begin
    if(reset) begin
        hi <= 0;
        lo <= 0;
    end
    else begin
        if(write_en) begin
            case (op)
                2'b00: begin // MTHI
                    hi <= in_1;
                end
                2'b01: begin // MTLO
                    lo <= in_1;
                end
                2'b10: begin // MULTU
                    hi <= prod[63:32];
                    lo <= prod[31:0];
                end
                2'b11: begin // DIVU
                    lo <= div[31:0];
                    hi <= rem[31:0];
                end
            endcase
        end
    end
end

endmodule