//control unit
module control_unit(
    input logic[5:0] opcode,
    input logic[3:0] state,
    input logic[5:0] fun,
    input logic[4:0] branchFunc,    //Could minimise to a single bit logic if needed (able to pass the test)
    input logic waitrequest,
    input logic[1:0] address_allign,

    /*
    certain outputs can remain constant as they do not alter the state of the cpu
    they may cause calculations to run but as long as nothing is stored anywhere it doesn't matter
    state based instruction require the cpu to be in a specific state to ensure functionallity
    this is resereved for anything writing
    due to using an avalon interface, uncecessary reads will also cause stalls in the cpu so these two nead to be state based
    */

    output logic[3:0] byteenable,
    output logic bytewrite,
    output logic halfwrite,
    
    output logic[3:0] ALUOp,    // constant     // see alu_contol for explanation of each possible value
    output logic      ALUSrc,   // constant     // if high ALU source is from instruction 16 bit imm, otherwise from registers
    output logic      singed_imm,// constant     // if high immediate treated as signed

    output logic      jump,     // constant     // if high jump to position from isntruction imediate, otherwise advance from pc+4
    output logic      branch,   // constant     // if high allow conditional branchin if alu has zero flag set to high. PC will coniditionally be set to PC+4 if not zero, and pc + instruction immediate otherwise

    output logic      memread,  // state based  // ask memroy for a read
    output logic      memwrite, // state based  // inform memory of a write

    output logic      regdst,   // constant     // changes the write register to be bits 15-11 of instr if high, and bits 25-21 otherwise
    output logic      memtoreg, // constant     // if high pass memroy to write data for register, othewise pass alu output
    output logic      regwrite,  // state based  // if high allow writign to register

    output logic      inwrite,  // state based  // if high allow writing to instr register
    output logic      pctoadd,   // constant     // if high pass PC to memory address otehrwise pass alu output

    output logic      pcwrite,
    output logic      regtojump,

    output logic      div_mult_en,
    output logic      div_mult_signed,
    output logic[1:0] div_mult_op,
    output logic      hitoreg,
    output logic      lotoreg,

    output logic      link,         //for the Return Address Register (linking instruction)
    output logic      reg_link,
    output logic      loadimmed,    //an extra signal for LUI

    output logic[2:0] ExtendOp      //for the Signextending 8bits/16bits word

);

// states
logic halt;
logic fetch;
logic decode;
logic exec1;
logic exec2;

// functions
logic arith;
logic regjump;
logic branchlink; 
logic mult_div;

always_comb begin
    //default settings for R-format instructions

    halt = state==0;
    fetch = state==1;
    decode = state==2;
    exec1 = state==3;
    exec2 = state==4;

    arith = fun == 6'b100001 | fun == 6'b100100 | fun == 6'b100101 | fun == 6'b101010 | fun == 6'b101011 | fun == 6'b100011 | fun == 6'b100110 | fun == 6'b000000 | fun == 6'b000011 | fun == 6'b000010 | fun == 6'b000100 | fun == 6'b000111 | fun == 6'b000110 | fun == 6'b010000 | fun == 6'b010010 | fun == 6'b001001;

    regjump = fun == 6'b001001 | fun == 6'b001000;
    branchlink = branchFunc == 5'b10001 || branchFunc == 5'b10000;
    mult_div = fun == 6'b010001 | fun == 6'b010011 | fun == 6'b011010 | fun == 6'b011011 | fun == 6'b011000 | fun == 6'b011001;

    
    pcwrite = exec2 & !waitrequest;

    if(fetch) begin // in fetch send pc to address
        byteenable = 4'b1111;
        bytewrite  = 0;
        halfwrite  = 0;
        ALUOp[3:0] = 4'b0000;
        ALUSrc     = 0;
        singed_imm = 0;
        jump       = 0;
        branch     = 0;
        memread    = 1;
        memwrite   = 0;
        regdst     = 0;
        memtoreg   = 0;
        regwrite   = 0;
        inwrite    = 0;
        pctoadd    = 1;
        regtojump  = 0;
        div_mult_en= 0; 
        div_mult_signed = 0;
        div_mult_op= 2'b00;
        hitoreg    = 0;
        lotoreg    = 0;
        link       = 0;
        reg_link   = 0;
        loadimmed  = 0;
        ExtendOp   = 3'b000;
    end
    else if(decode) begin // in decode store instruction
        byteenable = 4'b1111;
        bytewrite  = 0;
        halfwrite  = 0;
        ALUOp[3:0] = 4'b0000;
        ALUSrc     = 0;
        singed_imm = 0;
        jump       = 0;
        branch     = 0;
        memread    = 0;
        memwrite   = 0;
        regdst     = 0;
        memtoreg   = 0;
        regwrite   = 0;
        inwrite    = 1;
        pctoadd    = 1;
        regtojump  = 0;
        div_mult_en= 0;
        div_mult_signed = 0;
        div_mult_op= 2'b00;
        hitoreg    = 0;
        lotoreg    = 0;
        link       = 0;
        reg_link   = 0;
        loadimmed  = 0;
    end
    else begin
        case(opcode)
            /*
                I will break down the reason for every output value for the arithmetic instructions
                the same logic was used to deduce the outpu for every other instruction
            */
            6'b000000: begin /* REGISTER INSTR WITH FN AS DIFFERENCE */
                            // THIS INCLUDES ARITHLOG DIVMULT SHIFT SHIFTV JUMPMOVETO
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0010; // this is the alu control that tells the alu to process based on function field of instr
                ALUSrc     = 0; // the alu must read form register
                singed_imm = 0;
                jump       = regjump; // the pc must recieve data from normal +4 increment of pc unless we have a reg jump instr
                branch     = 0; // same as reason as above
                memread    = 0; // we don't want the cpu to stall for unecessary read
                memwrite   = 0; // we are not writing to the memory
                regdst     = 1; // we want to write to the register in bits 15-11 of the cpu
                memtoreg   = 0; // we want the alu data to be sent to the register write data
                regwrite   = arith & exec2; // we only want to allow writing to register when we can confirm correct data is in place
                inwrite    = 0; // we don't want instr register to be overwritten
                pctoadd    = 0; // we don't actually care what happens here
                regtojump  = regjump; // we want high on a register jump instr
                div_mult_en= mult_div & exec1;
                div_mult_signed = fun==6'b011010|fun==6'b011000;
                if(fun == 6'b010001) begin
                    div_mult_op = 2'b00;
                end
                else if(fun == 6'b010011) begin
                    div_mult_op = 2'b01;
                end
                else if(fun == 6'b011000 | fun == 6'b011001) begin
                    div_mult_op = 2'b10;
                end
                else if(fun == 6'b011010| fun == 6'b011011) begin
                    div_mult_op = 2'b11;
                end
                else begin
                    div_mult_op = 2'b00;
                end
                hitoreg    = fun == 6'b010000; // MFHI
                lotoreg    = fun == 6'b010010; // MFLO
                link       = fun == 6'b001001; // JALR
                reg_link   = fun == 6'b001001; // JALR
                loadimmed  = 0;  //we only use this signal when instruction is LUI
            end
            
            // ARITHLOGI
            6'b001001: begin // ADDIU
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000; 
                ALUSrc     = 1;
                singed_imm = 1;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end

            6'b001100: begin // ANDI !TODO: not yet tested
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0100;
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b001101: begin // ORI !TODO: not yet tested
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0101;
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b001010: begin // SLTI !TODO
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0111;
                ALUSrc     = 1;
                singed_imm = 1;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b001011: begin // SLTIU !TODO
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b1100;
                ALUSrc     = 1;
                singed_imm = 1;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b001110: begin // XORI !TODO: not yet tested
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0110;
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end

            //LOADI
            6'b001111: begin // LUI
            //similar to load, but require an extra signal to control the immed field
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000; //Don't care
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 1 & (exec1);
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = 1 & (exec2);
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 1; //extra signal

            end 
            

            // BRANCH
            6'b000100: begin // BEQ 
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0001;
                ALUSrc     = 0;
                singed_imm = 0;
                jump       = 0;
                branch     = 1;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = 0;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b000101: begin // BNE
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b1000; 
                ALUSrc     = 0;
                singed_imm = 0;
                jump       = 0;
                branch     = 1;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = 0;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end

            // BRANCHZ + OTHER BRANCHZ
            6'b000111: begin // BGTZ !TODO
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b1001;
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 1;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = 0;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b000110: begin // BLEZ !TODO
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b1010;
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 1;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = 0;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0; 
            end
            6'b000001: begin // BLTZ and OTHER_BRANCHZ 
                // BLTZ has function code of 00000
                // you'll have to differentiate the different function codes
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b1011;
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 1;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = exec2 & branchlink;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0; 
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = branchlink;
                loadimmed  = 0;
            end

    //100 for 16bits unsigned //101 for 16bits signed    
    //110 for 8bits   unished //111 for 8 bits signed
            // LOADSTORE
            6'b100000: begin // LB Load the LS byte in to dt
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000; 
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 1 & (exec1);
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = 1 & exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
                ExtendOp   = 3'b111;
            end
            6'b100100: begin // LBU !TODO
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000; 
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 1 & (exec1);
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = 1 & exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
                ExtendOp   = 3'b110;
            end
            6'b100001: begin // LH !TODO
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000; 
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 1 & (exec1);
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = 1 & exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
                ExtendOp   = 3'b101;
            end
            6'b100101: begin // LHU !TODO
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000; 
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 1 & (exec1);
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
                ExtendOp   = 3'b100;
            end
            6'b100011: begin // LW
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000; 
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 1 & (exec1);
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 1;
                regwrite   = 1 & exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b100010: begin // LWL !TODO
                byteenable = 4'b1111;
                ALUOp[3:0] = 4'b0000; 
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 1 & (exec1);
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = 1 & exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
                ExtendOp   = 3'b001; //Tempory usage 
            end
            6'b100110: begin // LWR !TODO
                byteenable = 4'b1111;
                ALUOp[3:0] = 4'b0000; 
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 1 & (exec1);
                memwrite   = 0;
                regdst     = 0;
                memtoreg   = 0;
                regwrite   = 1 & exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
                ExtendOp   = 3'b010; //Tempory usage 
            end
            6'b101000: begin // SB
                byteenable = 4'b0001 << address_allign;
                bytewrite  = 1;
                halfwrite  = 1;
                ALUOp[3:0] = 4'b0000;
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 1 & exec2;
                regdst     = 1; 
                memtoreg   = 0;
                regwrite   = 0;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b101001: begin // SH
                byteenable = 4'b0011 << address_allign;
                bytewrite  = 0;
                halfwrite  = 1;
                ALUOp[3:0] = 4'b0000;
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 1 & exec2;
                regdst     = 1; 
                memtoreg   = 0;
                regwrite   = 0;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b101011: begin // SW
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000;
                ALUSrc     = 1;
                singed_imm = 0;
                jump       = 0;
                branch     = 0;
                memread    = 0;
                memwrite   = 1 & exec2;
                regdst     = 1; 
                memtoreg   = 0;
                regwrite   = 0;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end

            // JUMP
            6'b000010: begin // J
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000;
                ALUSrc     = 0;
                singed_imm = 0;
                jump       = 1;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = 0;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 0;
                reg_link   = 0;
                loadimmed  = 0;
            end
            6'b000011: begin // JAL 
            /*Store the return address to Register 31 */
                byteenable = 4'b1111;
                bytewrite  = 0;
                halfwrite  = 0;
                ALUOp[3:0] = 4'b0000;
                ALUSrc     = 0;
                singed_imm = 0;
                jump       = 1;
                branch     = 0;
                memread    = 0;
                memwrite   = 0;
                regdst     = 0; 
                memtoreg   = 0;
                regwrite   = exec2;
                inwrite    = 0;
                pctoadd    = 0;
                regtojump  = 0;
                div_mult_en= 0; 
                div_mult_signed = 0;
                div_mult_op= 2'b00;
                hitoreg    = 0;
                lotoreg    = 0;
                link       = 1;
                reg_link   = 0;
                loadimmed  = 0;
            end
        endcase
    end
end

endmodule