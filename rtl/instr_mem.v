//Instruction memory
module instr_mem(
    input logic        clk,
    input logic[31:0]  address,

    output logic[31:0] inst
);

logic[31:0] Mem [0:127];

