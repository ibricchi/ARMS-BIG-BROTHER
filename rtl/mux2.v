//mux right before the ALU
module mux2(
    input logic[31:0]  readdata2,
    input logic[31:0]  extendoffset,
    input logic        ALUSrc,

    output logic[31:0] alu_b
);

always_comb begin
    if(ALUSrc == 0) begin
        alu_b = readdata2;
    end
    else begin
        alu_b = extendoffset;
    end
end

endmodule