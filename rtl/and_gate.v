//AND gate in the diagram: input from ALU zero and Branch
module and_gate(
    input logic branch,
    input logic zero,

    output logic out
);

always_comb begin
    out = branch && zero;
end

endmodule