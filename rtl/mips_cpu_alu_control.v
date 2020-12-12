module alu_control(
    input logic[3:0] ALUOp,
    input logic[5:0] FuncCode,
    input logic[4:0] BranchzFunc,

    output logic[4:0] ALUCtrl
);

logic[4:0] func;
logic[4:0] BranchCtrl;

always_comb begin
    case(FuncCode)
        6'b100001: func = 5'b00010; //addu
        6'b100011: func = 5'b00110; //subu
        6'b100101: func = 5'b00001; //or
        6'b100110: func = 5'b01101; //xor
        6'b100111: func = 5'b01100; //nor
        6'b101011: func = 5'b00111; //sltu
        6'b101010: func = 5'b01000; //slt
        6'b000000: func = 5'b01001; //sll
        6'b000011: func = 5'b01010; //sra
        6'b000010: func = 5'b01011; //srl
        6'b000100: func = 5'b01110; //sllv
        6'b000111: func = 5'b01111; //srav
        6'b000110: func = 5'b10000; //srlv

        default: func = 5'b00000;
    endcase
end

always_comb begin 
    case(BranchzFunc)
        5'b00001: BranchCtrl = 5'b11011; //BGEZ
        5'b10001: BranchCtrl = 5'b11011; //BGEZAL 
        5'b00000: BranchCtrl = 5'b11111; //BLTZ
        5'b10000: BranchCtrl = 5'b11111; //BLTZAL     
        default: BranchCtrl = 5'b00000;
    endcase
end

always_comb begin
    case(ALUOp)
        4'b0000: ALUCtrl = 5'b00010; //add
        4'b0001: ALUCtrl = 5'b00110; //sub
        4'b0010: ALUCtrl = func; //R-type instructions
        4'b0011: ALUCtrl = 5'b00010; //add
        4'b0100: ALUCtrl = 5'b00000; //Bitwise AND
        4'b0101: ALUCtrl = 5'b00001; //Bitwise OR
        4'b0110: ALUCtrl = 5'b01101; //Bitwise XOR
        4'b0111: ALUCtrl = 5'b01000; //SLT
        4'b1100: ALUCtrl = 5'b00111; //SLTU (It's not in order I know but we sorta ran out of spots for this)

        4'b1000: ALUCtrl = 5'b11000; //bne
        4'b1001: ALUCtrl = 5'b11001; //bgtz
        4'b1010: ALUCtrl = 5'b11010; //blez
        4'b1011: ALUCtrl = BranchCtrl; //BLTZ and OTHER_BRANCHZ instructions
        default: ALUCtrl = 0;
    endcase
end
       

endmodule