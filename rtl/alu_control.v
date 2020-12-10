module alu_control(
    input logic[3:0] ALUOp,
    input logic[5:0] FuncCode,
    input logic[4:0] BranchzFunc,

    output logic[3:0] ALUCtrl
);

logic[3:0] func;
logic[4:0] BranchCtrl;

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
    case(BranchzFunc)
        5'b00001: BranchCtrl = 4'b1011; //BGEZ
        // 5'b10001: BranchCtrl = 4'b; //BGEZAL TODO
        5'b00000: BranchCtrl = 4'b1111; //BLTZ
        // 5'b10000: BranchCtrl = 4'b; //BLTZAL TODO     
        default: BranchCtrl = 4'b0000;
    endcase
end

always_comb begin
    case(ALUOp)
        4'b0000: ALUCtrl = 4'b0010; //add
        4'b0001: ALUCtrl = 4'b0110; //sub
        4'b0010: ALUCtrl = func; //R-type instructions
        4'b0011: ALUCtrl = 4'b0010; //add

        4'b0100: ALUCtrl = 4'b0000; //Bitwise AND
        4'b0101: ALUCtrl = 4'b0001; //Bitwise OR
        4'b0110: ALUCtrl = 4'b1101; //Bitwise XOR
        4'b0111: ALUCtrl = 4'b0111; //SLT

        4'b1000: ALUCtrl = 4'b1000; //bne
        4'b1001: ALUCtrl = 4'b1001; //bgtz
        4'b1010: ALUCtrl = 4'b1010; //blez
        4'b1011: ALUCtrl = BranchCtrl; //BLTZ and OTHER_BRANCHZ instructions
        default: ALUCtrl = 0;
    endcase
end

endmodule