//control unit
module control_unit(
    input logic[5:0] opcode,
    input logic[3:0] state,
    
    output logic[1:0] ALUOp,    // constant
    output logic      ALUSrc,   // constant

    output logic      jump,     // constant
    output logic      branch,   // constant

    output logic      memread,  // state based
    output logic      memwrite, // state based

    output logic      regdst,   // constant
    output logic      memtoreg, // constant
    output logic      regwrite  // state based

    output logic      inwrite,  // state based
    output logic      pctoadd   // constant
);

logic halt;
logic fetch;
logic decode;
logic exec1;
logic exec2;

always_comb begin
    //default settings for R-format instructions

    halt = state==0;
    fetch = state==1;
    decode = state==2;
    exec1 = state==3;
    exec2 = state==4;

    if(fetch) begin // in fetch send pc to address
        instren = 0;
        pctoaddress = 1;
    end
    if(decode) begin // in decode store instruction
        instren = 1;
        pctoaddress = 1;
    end
    else begin
        case(opcode)
            6'b000000: begin /* arithmetic */  
                ALUOp[1:0] = 2'b10;
                ALUSrc     = 0;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 1; // reg dst is set 
                memtoreg   = 0;
                regwrite   = 1 & exec2;
                instren    = 0;
                pctoaddress= 0;
            end
            6'b100011: begin /* lw */
                ALUOp[1:0] = 2'b00;
                ALUSrc     = 1;
                jump       = 0;
                branch     = 0;
                memread    = 1 & exec1;
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 1;
                regwrite   = 1 & exec2;
                instren    = 0;
                pctoaddress= 0;
            end

            6'b101011: begin /* sw */
                ALUOp[1:0] = 2'b00;
                ALUSrc     = 1;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 1 & exec2;
                regdst     = 1; 
                memtoreg   = 0;
                regwrite   = 0;
                instren    = 0;
                pctoaddress= 0;
            end

            6'b000100: begin /* beq */
                ALUOp[1:0] = 2'b01;
                ALUSrc     = 0;
                jump       = 0;
                branch     = 1;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = 0;
                instren    = 0;
                pctoaddress= 0;
            end

            6'b000010: begin /* j */
                ALUOp[1:0] = 2'b00;
                ALUSrc     = 0;
                jump       = 1;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = 0;
                instren    = 0;
                pctoaddress= 0;
            end

            6'b000011: begin /* jal */
                ALUOp[1:0] = 2'b00;
                ALUSrc     = 0;
                jump       = 1;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = 1 & exec2;
                instren    = 0;
                pctoaddress= 0;
            end
        endcase
    end
end

endmodule


