module alu_control(
    input logic[1:0] ALUOp,
    input logic[5:0] FuncCode,

    output logic[3:0] ALUCtrl
);
