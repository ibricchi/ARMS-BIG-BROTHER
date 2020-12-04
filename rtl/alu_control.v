module alu_control(
    input logic[1:0] ALUOp,
    input logic[5:0] FuncCode,

    output logic[3:0] ALUCtrl
);

logic[3:0] func;

always_comb begin
    case(FuncCode)
        6'b100001: func = 4'b0010; //addu
        6'b100011: func = 4'b0110; //subu
        6'b100101: func = 4'b0001; //or
        6'b100110: func = 4'b1101; //xor
        6'b100111: func = 4'b1100; //nor
        6'b101010: func = 4'b0111; //slt
        default: func = 4'b0000;
    endcase
end

always_comb begin
    case(ALUOp)
        2'b00: ALUCtrl = 4'b0010; //add
        2'b01: ALUCtrl = 4'b0110; //sub
        2'b10: ALUCtrl = func; //R-type instructions
        2'b11: ALUCtrl = 4'b0010; //add
        default: ALUCtrl = 0;
    endcase
end

endmodule