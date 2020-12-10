//Instruction memory
module instr_mem(
    input logic        clk,
    input logic[31:0]  address,

    output logic[31:0] instr
);

logic[31:0] Mem [0:127];

initial begin
    Mem[0] = 32'h84210001;
    Mem[4] = 32'hAC010000;
    Mem[8] = 32'h8C240000;
    Mem[12] = 32'h10210001;
    Mem[16] = 32'h00411822;
end

/* Instructions to be tested are
add: R3,R1,R2
sw:  R1, 0(R0)
lw:  R4, 0(R1)
beq: R1,R1,8
add: R3,R0,R0
sub: R3,R2,R1
*/


always_ff @(posedge clk) begin
    instr <= Mem[address[31:2]];
end

endmodule