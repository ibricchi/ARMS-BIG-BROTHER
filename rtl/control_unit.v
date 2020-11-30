//control unit
module control_unit(
    input logic[5:0] opcode,
    
    output logic[1:0] ALUOp,
    output logic      ALUSrc,

    output logic      jump,
    output logic      branch,

    output logic      memread,
    output logic      memwrite,

    output logic      regdst,
    output logic      memtoreg,
    output logic      regwrite
);

always_comb begin
    //default settings for R-format instructions
    ALUOp[1:0] = 2'b10;
    ALUSrc     = 0;
    jump       = 0;
    branch     = 0;
    memread    = 0;
    memwrite   = 0;
    regdst     = 1;
    memtoreg   = 0;
    regwrite   = 1;

    case(opcode)
        6'b100011: begin /* lw */
            ALUOp[1:0] = 2'b00;
            ALUSrc     = 1;
            jump       = 0;
            branch     = 0;
            memread    = 1;
            memwrite   = 0;
            regdst     = 0;
            memtoreg   = 1;
            regwrite   = 1;
        end

        6'b101011: begin /* sw */
            ALUOp[1:0] = 2'b00;
            ALUSrc     = 1;
            jump       = 0;
            branch     = 0;
            memread    = 0;
            memwrite   = 1;
            /* regdst and memtoreg are X */
            regwrite   = 0;
        end

        6'b000100: begin /* beq */
            ALUOp[1:0] = 2'b01;
            ALUSrc     = 0;
            jump       = 0;
            branch     = 1;
            memread    = 0;
            memwrite   = 0;
            /* regdst and memtoreg are X */
            regwrite   = 0;
        end

        6'b000010: begin /* j */
            jump       = 1;
            branch     = 0;
            memread    = 0;
            memwrite   = 0;
            regwrite   = 0;
        end

        6'b000011: begin /* jal */
            jump       = 1;
            branch     = 0;
            memread    = 0;
            memwrite   = 0;
            regwrite   = 1;
        end

        


    endcase

end

endmodule


