module mips_cpu_bus(
    /* Standard signals */
    input logic clk,
    input logic reset,
    output logic active,
    output logic[31:0] register_v0,

    /* Avalon memory mapped bus controller (master) */
    output logic[31:0] address,
    output logic write,
    output logic read,
    input logic waitrequest,
    output logic[31:0] writedata,
    output logic[3:0] byteenable,
    input logic[31:0] readdata
);

// setup state machine
logic[3:0] state;
initial begin
    state = 0;
    active = 0;
end

always_ff @(posedge clk) begin // on every clock cycle if waitrequest is low change state
    // debug code
    // $display("Instruction: %h", instr, " PC: ", pc_out - 3217031168, " regsiter: %d", write_reg);
    if(!waitrequest) case(state)
        0: begin // HALT
            state <= 1;
            active <= 1;
            // $display("Entering FETCH STATE: ");
        end
        1: begin // FETCH
            state <= 2;
            // $display("Entering DECO STATE: ");
        end
        2: begin // DECODE
            state <= 3;
            // $display("Entering EXEC1 STATE: ");
        end
        3: begin // EXEC1
            state <= 4;
            // $display("Entering EXEC2 STATE: ");
        end
        4: begin // EXEC2
            state <= 1;
            // $display("Entering FETCH STATE: ");
        end
    endcase
end

always_ff @(posedge clk) begin // check if pc is at 0 and terminate
    if(state!=0 & pc_out == 0) begin
        active <= 0;
        state <= 0; // halt
    end
end

//instruction register not yet implemented
//here I just created a logic 32-bit component as instruction
logic[31:0] instr, instr_reg;
logic[31:0] pc_in, pc_out;
logic pcwrite;

pc pc_0(
    .clk(clk),
    .reset(reset),
    .pc_in(pc_in),
    .pcwrite(pcwrite),
    .pc_out(pc_out)
);

//control unit (not updated yet)
logic[3:0] ALUOp;
logic[1:0] div_mult_op;
logic[2:0] ExtendOp;
logic bytewrite, halfwrite, ALUSrc, singed_imm, jump, branch, regdst, memtoreg, regwrite, inwrite, pctoadd, regtojump, div_mult_en, div_mult_signed, hitoreg, lotoreg, link, reg_link, loadimmed;

control_unit control_0(
    .opcode(instr[31:26]),
    .state(state),
    .fun(instr[5:0]),
    .branchFunc(instr[20:16]),
    .waitrequest(waitrequest),
    .address_allign(address_internal[1:0]),

    .byteenable(byteenable),
    .bytewrite(bytewrite),
    .halfwrite(halfwrite),
    .ALUOp(ALUOp),
    .ALUSrc(ALUSrc),
    .singed_imm(singed_imm),
    .jump(jump),
    .branch(branch),
    .memread(read),
    .memwrite(write),
    .regdst(regdst),
    .memtoreg(memtoreg),
    .regwrite(regwrite),
    .inwrite(inwrite),
    .pctoadd(pctoadd),
    .pcwrite(pcwrite),
    .regtojump(regtojump),
    .div_mult_en(div_mult_en),
    .div_mult_signed(div_mult_signed),
    .div_mult_op(div_mult_op),
    .hitoreg(hitoreg),
    .lotoreg(lotoreg),
    .link(link),
    .reg_link(reg_link),
    .loadimmed(loadimmed),
    .ExtendOp(ExtendOp)
);

// instr register
always_ff @(posedge clk) begin
    if(inwrite) begin
        instr_reg <= readdata;
    end
end
assign instr = (state==2)?readdata:instr_reg;

//register file
logic[31:0] read_data1, read_data2;
logic[4:0] write_reg;
logic[31:0] write_data;
logic branch_regwrite;
assign write_reg = (regdst == 0) ? ((link&!reg_link)? 5'b11111:instr[20:16]) : instr[15:11];

register_file reg_file_0(
    .clk(clk),
    .reset(reset),

    .read_index1(instr[25:21]),
    .read_index2(instr[20:16]),
    .write_enable(regwrite),
    .write_reg(write_reg),
    .write_data(write_data),

    .read_data1(read_data1),
    .read_data2(read_data2),
    .register_v0(register_v0)
);

// multiply and divide alu and register
logic[31:0] hi, lo;
div_mult_reg div_mult_reg_0(
    .clk(clk),
    .reset(reset),

    .write_en(div_mult_en),
    .sin(div_mult_signed),
    .op(div_mult_op),
    .in_1(read_data1),
    .in_2(read_data2),

    .hi(hi),
    .lo(lo)
);


logic[31:0] unextend_out, extend_out;
assign unextend_out = {16'h0000, instr[15:0]};
assign extend_out = {{16{instr[15]}}, instr[15:0]};

logic[31:0] alu_b;
assign alu_b = (ALUSrc == 0) ? read_data2 : (singed_imm?extend_out:unextend_out);

//ALU Control
logic[4:0] ALUCtrl;
alu_control alu_ctrl_0(
    .ALUOp(ALUOp),
    .FuncCode(instr[5:0]),
    .ALUCtrl(ALUCtrl),
    .BranchzFunc(instr[20:16])
);

//ALU
logic zero;
logic[31:0] ALU_out;

alu alu_0(
    .a(read_data1),
    .b(alu_b),
    .as(instr[10:6]),
    .alu_control(ALUCtrl),
    .result(ALU_out),
    .zero(zero)
);

// pc_calculated on branch
logic[31:0] add_out;
assign add_out = pc_out + (extend_out << 2);

logic and_result;
assign and_result = branch && zero;

logic delay_ctrl;
logic[31:0] delay_address;
delay_reg delay_reg0(
    .clk(clk),
    .add_out(add_out),
    .and_result(and_result),
    .jump(jump),
    .jump_address((regtojump ? read_data1 : {pc_out[31:28],{2'b00,instr[25:0]}<<2})-4),
    .exec1(state ==3),
    .exec2(state ==4),
    .delay_address(delay_address),
    .delay_ctrl(delay_ctrl)
);

//MUX3
logic[31:0] loadresult;
assign loadresult = {instr[15:0],16'h0000};

//MUX4 Location
assign pc_in = (delay_ctrl ? delay_address : pc_out)+4;
//from data memory
logic[31:0] address_internal;
assign address_internal = pctoadd?pc_out:ALU_out;
assign address = {address_internal[31:2],2'b00};

//writedata always second output of register shifted based on address
logic[31:0] half_shifter, byte_shifter;
assign half_shifter = (halfwrite & address_internal[1])?(read_data2<<16):read_data2;
assign byte_shifter = (bytewrite & address_internal[0])?(half_shifter<<8):half_shifter;
assign writedata = byte_shifter;


//I really hate this way of implementing this function, but selection in wire (like instr[5:2]) in not allow in always_comb
logic[31:0] ExtendRes1,ExtendRes2,ExtendRes3,ExtendRes0;
logic[15:0] HalfRes;

assign HalfRes = address_internal[1]?readdata[31:16]:readdata[15:0];

logic[7:0] byteRes;

assign byteRes = address_internal[0]?HalfRes[15:8]:HalfRes[7:0];

assign ExtendRes3 = {{25{byteRes[7]}}, byteRes[6:0] };    //LB
assign ExtendRes2 = {24'h000000,byteRes[7:0]};          //LBU
assign ExtendRes1 = { {17{HalfRes[15]}}, HalfRes[14:0] }; //LH
assign ExtendRes0 = {16'h0000,HalfRes[15:0]};           //LHUs

logic[31:0] LWL_result, LWR_result;

integer LWL_index;
assign LWL_index = 8*address_internal[0]+16*address_internal[1];
assign LWL_result = (readdata[31:0] <<(24-LWL_index)) + (read_data2[31:0] <<(8+LWL_index) >>(8+LWL_index));
assign LWR_result = (readdata[31:0] >>LWL_index) + (read_data2[31:0] >>(32-LWL_index) <<(32-LWL_index));

always_comb begin
    if(memtoreg) begin
        write_data = readdata;
    end
    else if(link) begin
        write_data = pc_out+8;
    end
    else if(loadimmed) begin
        write_data = loadresult;
    end
    else if(hitoreg) begin
        write_data = hi;
    end
    else if(lotoreg) begin
        write_data = lo;
    end
    else if(ExtendOp != 0)begin 
        write_data = ExtendRes1;
        case(ExtendOp)
            3'b001:write_data = LWL_result;
            3'b010:write_data = LWR_result;
            3'b100:write_data = ExtendRes0;
            3'b101:write_data = ExtendRes1;
            3'b110:write_data = ExtendRes2;
            3'b111:write_data = ExtendRes3;
        endcase 
    end 
    else begin
        write_data = ALU_out;
    end
end

endmodule
