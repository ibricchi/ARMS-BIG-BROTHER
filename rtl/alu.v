module alu(
    input logic[31:0]  a,
    input logic[31:0]  b,
    input logic[3:0]   alu_control,

    output logic[31:0] result,
    output logic       zero
);

always_comb begin
    case(alu_control)
        4'b0000: result = a & b; //AND
        4'b0001: result = a | b; //OR
        4'b0010: result = a + b; //ADD
        4'b0110: result = a - b; //SUB
        4'b0111: result = (a < b) ? 1:0; //SLT (set on less than)
        4'b1100: result = ~(a | b);//NOR
        4'b1101: result = a ^ b; //XOR
        default: result = 0;
    endcase
end

assign zero = (result == 0);

endmodule