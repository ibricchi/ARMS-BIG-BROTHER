//An ADD ALU situated at the top of the diagram
module add_alu(
    input logic[31:0] pc_out,
    input logic[31:0] shift_out,

    output logic[31:0] out
);

always_comb begin
    out = pc_out + shift_out;
end

endmodule