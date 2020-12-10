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
        
        4'b1000: result = (a != b) ? 0:1; //BNE(set on equal)
        4'b1001: result = ($signed(a) > 0) ? 0:1; //BGTZ(set on if a greater than 0)
        4'b1010: result = ($signed(a) <= 0) ? 0:1; //BLEZ(set on if a less than or equal to 0)
        4'b1011: result = ($signed(a) >= 0) ? 0:1; //BGEZ(set on if a greater than or equal to 0)
        4'b1111: result = ($signed(a) < 0) ? 0:1; //BLTZ(set on if a less than 0)
        default: result = 0;
    endcase
end

assign zero = (result == 0);

endmodule