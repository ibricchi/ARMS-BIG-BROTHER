module alu(
    input logic[31:0]  a,
    input logic[31:0]  b,
    input logic[4:0]   as,
    input logic[4:0]   alu_control,

    output logic[31:0] result,
    output logic       zero
);

always_comb begin
    case(alu_control)
        5'b00000: result = a & b; //AND
        5'b00001: result = a | b; //OR
        5'b00010: result = a + b; //ADD
        5'b00110: result = a - b; //SUB
        5'b00111: result = $unsigned(a) < $unsigned(b); //SLTU (set on less than)
        5'b01000: result = $signed(a)<$signed(b); // SLT
        5'b01001: result = b << as; // SLL
        5'b01010: result = $signed(b) >>> as; // SRA
        5'b01011: result = b >> as; // SRL 
        5'b01100: result = ~(a | b);//NOR
        5'b01101: result = a ^ b; //XOR
        5'b01110: result = b << a; // SLLV
        5'b01111: result = $signed(b) >>> a; // SRAV
        5'b10000: result = b >> a; // SRLV

        5'b11000: result = (a != b) ? 0:1; //BNE(set on equal)
        5'b11001: result = ($signed(a) > 0) ? 0:1; //BGTZ(set on if a greater than 0)
        5'b11010: result = ($signed(a) <= 0) ? 0:1; //BLEZ(set on if a less than or equal to 0)
        5'b11011: result = ($signed(a) >= 0) ? 0:1; //BGEZ(set on if a greater than or equal to 0)
        5'b11111: result = ($signed(a) < 0) ? 0:1; //BLTZ(set on if a less than 0)
        default: result = 0;
    endcase
end

assign zero = (result == 0);

endmodule