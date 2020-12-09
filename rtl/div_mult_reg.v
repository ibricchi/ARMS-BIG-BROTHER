//An ADD ALU situated at the top of the diagram
module div_mult_reg(
    input logic clk,
    input logic reset,

    input logic write_en,
    input logic sin,
    input logic[2:0] op,
    input logic[31:0] in_1,
    input logic[31:0] in_2,

    output logic[31:0] hi,
    output logic[31:0] lo
);

logic unsigned[63:0] in_1_u;
logic unsigned[63:0] in_2_u;
logic unsigned[63:0] prod_u;
logic unsigned[31:0] div_u;
logic unsigned[31:0] rem_u;
assign in_1_u = in_1;
assign in_2_u = in_2;
assign prod_u = in_1_u * in_2_u;
assign div_u = in_1_u / in_2_u;
assign rem_u = in_1_u % in_2_u;

logic signed[63:0] in_1_s;
logic signed[63:0] in_2_2;
logic signed[63:0] prod_s;
logic signed[31:0] div_s;
logic signed[31:0] rem_s;
assign in_1_s = in_1;
assign in_2_s = in_2;
assign prod_s = in_1_s * in_2_s;
assign div_s = in_1_s / in_2_s;
assign rem_s = in_1_s % in_2_s;

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
                hi <= sin?prod_s[63:32]:prod_u[63:32];
                lo <= sin?prod_s[31:0]:prod_u[31:0];
            end
            2'b11: begin // DIVU
                lo <= sin?div_s:div_u;
                hi <= sin?rem_s:rem_u;
            end
        endcase
    end
end

endmodule