//mux right before the register file
module mux1(
    input logic[20:16] instr20_16,
    input logic[15:11] instr15_11,
    input logic        regdst,

    output logic[4:0]  writereg
);

always_comb begin
    if(regdst == 0) begin
        writereg = instr20_16;
    end
    else begin
        writereg = instr15_11;
    end
end

endmodule